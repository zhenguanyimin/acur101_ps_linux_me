
#ifndef FLASH_RAW_H
#define FLASH_RAW_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

/**
 *  @b Description
 *  @n
 *      Init FlashRaw module. None of the FlashRaw API's can be used without invoking this API.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Init(void);

/**
 *  @b Description
 *  @n
 *      Set a NV item to flash.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @param[in]  value_buf
 *      Buffer address of the contents.
 *
 *  @param[in]  buf_len
 *      Length of the contents in the buffer.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Set(const char *key, const void *value_buf, size_t buf_len);

/**
 *  @b Description
 *  @n
 *      Get a NV item from flash.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @param[in]  value_buf
 *      Buffer address used to store the contents.
 *
 *  @param[in]  buf_len
 *      Length of the buffer.
 *
 *  @param[out]  saved_value_len
 *      Length of the NV contents in the flash.
 *
 *  @retval
 *      Length of data store in the buffer.
 */
size_t FlashRaw_Get(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len);

/**
 *  @b Description
 *  @n
 *      Delete a NV item.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Del(const char *key);

#endif /* FLASH_RAW_H */
