
#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stddef.h>
#include <limits>
#include <atomic>

/*!
 * \brief Lock free, with no wasted slots ringbuffer implementation. Insert data into head and get data from tail
 *
 * \tparam T Type of buffered elements
 * \tparam buffer_size Size of the buffer. Must be a power of 2.
 * \tparam wmo_multi_core Generate extra memory barrier instructions in case of core to core communication
 * \tparam cacheline_size Size of the cache line, to insert appropriate padding in between indexes and buffer
 * \tparam index_t Type of array indexing type. Serves also as placeholder for future implementations.
 */
template<typename T, size_t buffer_size = 1024, bool wmo_multi_core = true, size_t cacheline_size = 0, typename index_t = size_t>
	class Ringbuffer
	{
	public:
		/*!
	 	 * \brief Default constructor, will initialize head and tail indexes
	 	 */
		Ringbuffer() : head(0), tail(0) {}

		/*!
		 * \brief Special case constructor to premature out unnecessary initialization code when object is
		 * instatiated in .bss section
		 * \warning If object is instantiated on stack, heap or inside noinit section then the buffer have to be
	 	 * explicitly cleared before use
		 * \param dummy Ignored
		 */
		Ringbuffer(int dummy) { (void)(dummy); }

		/*!
		 * \brief Clear buffer from producer side
		 */
		void producerClear(void) {
			//this may fail
			head.store(tail.load(std::memory_order_relaxed), std::memory_order_relaxed);
			//head.store(tail.load(std::memory_order_relaxed), index_release_barrier);
		}

		/*!
		 * \brief Clear buffer from consumer side
		 */
		void consumerClear(void) {
			tail.store(head.load(std::memory_order_relaxed), std::memory_order_relaxed);
			//tail.store(head.load(std::memory_order_relaxed), index_release_barrier);
		}

		/*!
		 * \brief Check if buffer is empty
		 * \return True if buffer is empty
		 */
		bool isEmpty(void) const {
			return readAvailable() == 0;
		}

		/*!
		 * \brief Check if buffer is full
		 * \return True if buffer is full
		 */
		bool isFull(void) const {
			return writeAvailable() == 0;
		}

		/*!
		 * \brief Check how many elements can be read from the buffer
		 * \return Number of elements that can be read
		 */
		index_t readAvailable(void) const {
			return head.load(index_acquire_barrier) - tail.load(std::memory_order_relaxed);
		}

		/*!
		 * \brief Check how many elements can be written into the buffer
		 * \return Number of free slots that can be be written
		 */
		index_t writeAvailable(void) const {
			return buffer_size - (head.load(std::memory_order_relaxed) - tail.load(index_acquire_barrier));
		}

		/*!
		 * \brief Inserts data into internal buffer, without blocking
		 * \param data element to be inserted into internal buffer
		 * \return True if data was inserted
		 */
		bool insert(T data)
		{
			index_t tmp_head = head.load(std::memory_order_relaxed);

			if((tmp_head - tail.load(index_acquire_barrier)) == buffer_size)
				return false;
			else
			{
				data_buff[tmp_head++ & buffer_mask] = data;
				std::atomic_signal_fence(std::memory_order_release);
				head.store(tmp_head, index_release_barrier);
			}
			return true;
		}

		/*!
		 * \brief Inserts data into internal buffer, without blocking
		 * \param[in] data Pointer to memory location where element, to be inserted into internal buffer, is located
		 * \return True if data was inserted
		 */
		bool insert(const T* data)
		{
			index_t tmp_head = head.load(std::memory_order_relaxed);

			if((tmp_head - tail.load(index_acquire_barrier)) == buffer_size)
				return false;
			else
			{
				data_buff[tmp_head++ & buffer_mask] = *data;
				std::atomic_signal_fence(std::memory_order_release);
				head.store(tmp_head, index_release_barrier);
			}
			return true;
		}

		/*!
		 * \brief Inserts data returned by callback function, into internal buffer, without blocking
		 *
		 * This is a special purpose function that can be used to avoid redundant availability checks in case when
		 * acquiring data have a side effects (like clearing status flags by reading a peripheral data register)
		 *
		 * \param get_data_callback Pointer to callback function that returns element to be inserted into buffer
		 * \return True if data was inserted and callback called
		 */
		bool insertFromCallbackWhenAvailable(T (*get_data_callback)(void))
		{
			index_t tmp_head = head.load(std::memory_order_relaxed);

			if((tmp_head - tail.load(index_acquire_barrier)) == buffer_size)
				return false;
			else
			{
				//execute callback only when there is space in buffer
				data_buff[tmp_head++ & buffer_mask] = get_data_callback();
				std::atomic_signal_fence(std::memory_order_release);
				head.store(tmp_head, index_release_barrier);
			}
			return true;
		}

		/*!
		 * \brief Removes single element without reading
		 * \return True if one element was removed
		 */
		bool remove()
		{
			index_t tmp_tail = tail.load(std::memory_order_relaxed);

			if(tmp_tail == head.load(std::memory_order_relaxed))
				return false;
			else
				tail.store(++tmp_tail, index_release_barrier); // release in case data was loaded/used before

			return true;
		}

		/*!
		 * \brief Removes multiple elements without reading and storing it elsewhere
		 * \param cnt Maximum number of elements to remove
		 * \return Number of removed elements
		 */
		size_t remove(size_t cnt) {
			index_t tmp_tail = tail.load(std::memory_order_relaxed);
			index_t avail = head.load(std::memory_order_relaxed) - tmp_tail;

			cnt = (cnt > avail) ? avail : cnt;

			tail.store(tmp_tail + cnt, index_release_barrier);
			return cnt;
		}

		/*!
		 * \brief Reads one element from internal buffer without blocking
		 * \param[out] data Reference to memory location where removed element will be stored
		 * \return True if data was fetched from the internal buffer
		 */
		bool remove(T& data) {
			return remove(&data); // references are anyway implemented as pointers
		}

		/*!
		 * \brief Reads one element from internal buffer without blocking
		 * \param[out] data Pointer to memory location where removed element will be stored
		 * \return True if data was fetched from the internal buffer
		 */
		bool remove(T* data) {
			index_t tmp_tail = tail.load(std::memory_order_relaxed);

			if(tmp_tail == head.load(index_acquire_barrier))
				return false;
			else
			{
				*data = data_buff[tmp_tail++ & buffer_mask];
				std::atomic_signal_fence(std::memory_order_release);
				tail.store(tmp_tail, index_release_barrier);
			}
			return true;
		}

		/*!
		 * \brief Gets the first element in the buffer on consumed side
		 *
		 * It is safe to use and modify item contents only on consumer side
		 *
		 * \return Pointer to first element, nullptr if buffer was empty
		 */
		T* peek() {
			index_t tmp_tail = tail.load(std::memory_order_relaxed);

			if(tmp_tail == head.load(index_acquire_barrier))
				return nullptr;
			else
				return &data_buff[tmp_tail & buffer_mask];
		}

		/*!
		 * \brief Gets the first element in the buffer on consumed side and remove it
		 *
		 * It is safe to use and modify item contents only on consumer side
		 *
		 * \return Pointer to first element, nullptr if buffer was empty
		 */
		T* pop() {
			index_t tmp_tail = tail.load(std::memory_order_relaxed);

			T* buff = nullptr;
			if (tmp_tail == head.load(index_acquire_barrier))
				return nullptr;
			else
			{
				buff = &data_buff[tmp_tail & buffer_mask];
				tmp_tail++;
				std::atomic_signal_fence(std::memory_order_release);
				tail.store(tmp_tail, index_release_barrier);
				return buff;

			}
		}

		/*!
		 * \brief Gets the n'th element on consumed side
		 *
		 * It is safe to use and modify item contents only on consumer side
		 *
		 * \param index Item offset starting on the consumed side
		 * \return Pointer to requested element, nullptr if index exceeds storage count
		 */
		T* at(size_t index) {
			index_t tmp_tail = tail.load(std::memory_order_relaxed);

			if((head.load(index_acquire_barrier) - tmp_tail) <= index)
				return nullptr;
			else
				return &data_buff[(tmp_tail + index) & buffer_mask];
		}

		/*!
		 * \brief Gets the n'th element on consumed side
		 *
		 * Unchecked operation, assumes that software already knows if the element can be used, if
		 * requested index is out of bounds then reference will point to somewhere inside the buffer
		 * The isEmpty() and readAvailable() will place appropriate memory barriers if used as loop limiter
		 * It is safe to use and modify T contents only on consumer side
		 *
		 * \param index Item offset starting on the consumed side
		 * \return Reference to requested element, undefined if index exceeds storage count
		 */
		T& operator[](size_t index) {
			return data_buff[(tail.load(std::memory_order_relaxed) + index) & buffer_mask];
		}

	protected:
		constexpr static index_t buffer_mask = buffer_size-1; //!< bitwise mask for a given buffer size
		constexpr static std::memory_order index_acquire_barrier = wmo_multi_core ?
				  std::memory_order_acquire // do not load from, or store to buffer before confirmed by the opposite side
				: std::memory_order_relaxed;
		constexpr static std::memory_order index_release_barrier = wmo_multi_core ?
				  std::memory_order_release // do not update own side before all operations on data_buff committed
				: std::memory_order_relaxed;

		alignas(cacheline_size) std::atomic<index_t> head; //!< head index
		alignas(cacheline_size) std::atomic<index_t> tail; //!< tail index

		// put buffer after variables so everything can be reached with short offsets
		alignas(cacheline_size) T data_buff[buffer_size]; //!< actual buffer

	private:
		// let's assert that no UB will be compiled in
		static_assert((buffer_size != 0), "buffer cannot be of zero size");
		static_assert((buffer_size & buffer_mask) == 0, "buffer size is not a power of 2");
		static_assert(sizeof(index_t) <= sizeof(size_t),
			"indexing type size is larger than size_t, operation is not lock free and doesn't make sense");

		static_assert(std::numeric_limits<index_t>::is_integer, "indexing type is not integral type");
		static_assert(!(std::numeric_limits<index_t>::is_signed), "indexing type shall not be signed");
		//static_assert(buffer_mask <= (std::numeric_limits<index_t>::max() >> 1),
		//	"buffer size is too large for a given indexing type (maximum size for n-bit type is 2^(n-1))");
	};
#endif //RINGBUFFER_HPP
