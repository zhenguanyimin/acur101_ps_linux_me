#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
//#include <asm/system.h>

#define SIMPLE_DCACHE_SET  _IO('s', 0)
#define SIMPLE_DCACHE_UNSET  _IO('s', 1)
#define SIMPLE_DCACHE_FLUSH_RANGE  _IO('s', 2)
#define DEVICE_NAME "simple-dcache"

#define INVALIDATE_MEMORY_START	0x60000000
#define INVALIDATE_MEMORY_LEN	(2*1024*1024 * 10)
static dev_t dev;
static struct cdev cdev;
static struct class *dev_class;

#ifdef mtcpdc
#undef mtcpdc
#endif
#define mtcpdc(reg,val)	__asm__ __volatile__("dc " #reg ",%x0" : : "r" (val))
#define VERSAL_NET

#ifdef dsb
#undef dsb
#endif
/* Data Synchronization Barrier */
#define dsb() __asm volatile ("dmb sy" ::: "memory")

struct dcache_range_info {
	unsigned long paddr;
	unsigned long len;
};


static int simple_dcache_open(struct inode *inode, struct file *file)
{
	(void)inode;
	(void)file;

	return 0;
}

static int simple_dcache_release(struct inode *inode, struct file *file)
{
	(void)inode;
	(void)file;

	return 0;
}

static int set_memory_invalidate_dcache(unsigned long paddr, unsigned long len)
{
	const unsigned long cacheline = 64U;
	unsigned int *vaddr = phys_to_virt(paddr);
	unsigned long adr = (unsigned long)vaddr;
	unsigned int end = adr + len;

	adr = adr & (~0x3F);
	//u32 currmask = mfcpsr();
	//mtcpsr(currmask | IRQ_FIQ_MASK);

	//printk("adr:%x, len:%d\n", adr, len);
	local_irq_disable();

	if (len != 0U) {
		while (adr < end) {
			mtcpdc(CIVAC,adr);
			adr += cacheline;
#if defined (VERSAL_NET)
			/*
			 * Note: "NOP" instructions added below is workaround to avoid processor
			 * halts observed during execution of cache integration test. Issue is
			 * observed with O2 optimization level, adding 60 NOP instructions between
			 * consecutive CIVAC instruction prevents it.  Below are the observations,
			 *   - Issue observed only with cache integration test, which does stress
			 *     testing on cache APIs
			 *   - Issue was not reproduced with O0 optimization level
			 *   - This issue has been observed from IPP_PSXL_0_9_1 release.
			 * These NOP instruction would be removed, once said issue is resolved.
			 */
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
#endif
		}
	}
	/* Wait for invalidate to complete */
	dsb();
	local_irq_enable();
	//	mtcpsr(currmask);

	//printk("end\n");

	return 0;
}

static long simple_dcache_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	struct dcache_range_info dri = {0}; 
	struct dcache_range_info *udri = (struct dcache_range_info*)arg;
	(void)filp;
	(void)cmd;
	(void)arg;

	switch(cmd) {
		case SIMPLE_DCACHE_SET:
			ret = set_memory_invalidate_dcache(INVALIDATE_MEMORY_START, INVALIDATE_MEMORY_LEN);
			break;
		case SIMPLE_DCACHE_UNSET:
			//	set_memory_invalidate_dcache();
			ret = -1;
			break;
		case SIMPLE_DCACHE_FLUSH_RANGE:
			if(NULL == udri) {
				ret = -EINVAL;
			} else {
				if(copy_from_user(&dri, udri, sizeof(struct dcache_range_info))) {
					ret = -EFAULT;
				} else {
					ret = set_memory_invalidate_dcache(dri.paddr, dri.len);
				}
			}

			break;
		default:

			break;

	}

	return ret;
}

static const struct file_operations simple_dcache_fops = {
	.owner = THIS_MODULE,
	.open = simple_dcache_open,
	.release = simple_dcache_release,
	.unlocked_ioctl = simple_dcache_ioctl,
};

static int simple_dcache_probe(void)
{

	dev = MKDEV(1, 105);
	cdev_init(&cdev, &simple_dcache_fops);
	if (cdev_add(&cdev, dev, 1) < 0) {
		printk("Failed to add character device\n");
		unregister_chrdev_region(dev, 1);
		return -ENODEV;
	}

	dev_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(dev_class)) {
		printk("Failed to create device class\n");
		cdev_del(&cdev);
		unregister_chrdev_region(dev, 1);
		return -ENODEV;
	}

	if (device_create(dev_class, NULL, dev, NULL, DEVICE_NAME) == NULL) {
		printk("Failed to create device\n");
		class_destroy(dev_class);
		cdev_del(&cdev);
		unregister_chrdev_region(dev, 1);
		return -ENODEV;
	}

	printk("simple dcache initialized\n");

	return 0;
}

static int simple_dcache_remove(void)
{
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, 1);

	return 0;
}

static int __init simple_dcache_init(void)
{
	return simple_dcache_probe();
}

static void __exit simple_dcache_exit(void)
{
	simple_dcache_remove();	
}

module_init(simple_dcache_init);
module_exit(simple_dcache_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tangsongquan");
MODULE_DESCRIPTION("simple dcache Driver");	


