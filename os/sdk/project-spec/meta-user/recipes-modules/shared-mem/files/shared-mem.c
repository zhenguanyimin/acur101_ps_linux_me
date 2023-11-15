#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/dma-mapping.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>

#define DEVICE_NAME "shared-mem"

static dev_t dev;
static struct cdev cdev;
static struct class *dev_class;
static void __iomem *mem_start;
static ssize_t mem_size;

static int shared_mem_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int shared_mem_release(struct inode *inode, struct file *file)
{
    return 0;
}

static loff_t shared_mem_llseek(struct file *file, loff_t offset, int whence)
{
    loff_t newpos;

    switch (whence) {
        case 0: /* SEEK_SET */
            newpos = offset;
            break;

        case 1: /* SEEK_CUR */
            newpos = file->f_pos + offset;
            break;

        case 2: /* SEEK_END */
            newpos = mem_size + offset;
            break;

        default: /* can't happen */
            return -EINVAL;
    }

    if (newpos < 0 || newpos > mem_size)
        return -EINVAL;

    file->f_pos = newpos;
    return newpos;
}

static ssize_t shared_mem_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    if (*pos >= mem_size)
        return 0;

    if (*pos + count > mem_size)
        count = mem_size - *pos;

    if (copy_to_user(buf, mem_start + *pos, count))
        return -EFAULT;

    *pos += count;
    return count;
}

static ssize_t shared_mem_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
    if (*pos >= mem_size)
        return 0;

    if (*pos + count > mem_size)
        count = mem_size - *pos;

    if (copy_from_user(mem_start + *pos, buf, count))
        return -EFAULT;

    *pos += count;
    return count;
}

static int shared_mem_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long size = vma->vm_end - vma->vm_start;
    unsigned long pfn = virt_to_phys(mem_start) >> PAGE_SHIFT;

    if (size > mem_size)
        return -EINVAL;

    if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot))
        return -EAGAIN;

    return 0;
}

static const struct file_operations shared_mem_fops = {
    .owner = THIS_MODULE,
    .open = shared_mem_open,
    .release = shared_mem_release,
    .llseek = shared_mem_llseek,
    .read = shared_mem_read,
    .write = shared_mem_write,
    .mmap = shared_mem_mmap,
};

static int shared_mem_probe(struct platform_device *pdev)
{
    struct resource *res;
    struct device_node *np = pdev->dev.of_node;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "Failed to get memory resource\n");
        return -ENODEV;
    }

	mem_start = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(mem_start)) {
        dev_err(&pdev->dev, "Failed to remap memory resource\n");
        return PTR_ERR(mem_start);
    }

    mem_size = resource_size(res);

	/*
    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0) {
        dev_err(&pdev->dev, "Failed to allocate device number\n");
        return -ENODEV;
    }
    */
    dev = MKDEV(1, 35);

    cdev_init(&cdev, &shared_mem_fops);
    if (cdev_add(&cdev, dev, 1) < 0) {
        dev_err(&pdev->dev, "Failed to add character device\n");
        unregister_chrdev_region(dev, 1);
        return -ENODEV;
    }

    dev_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(dev_class)) {
        dev_err(&pdev->dev, "Failed to create device class\n");
        cdev_del(&cdev);
        unregister_chrdev_region(dev, 1);
        return -ENODEV;
	}

    if (device_create(dev_class, NULL, dev, NULL, DEVICE_NAME) == NULL) {
        dev_err(&pdev->dev, "Failed to create device\n");
        class_destroy(dev_class);
        cdev_del(&cdev);
        unregister_chrdev_region(dev, 1);
        return -ENODEV;
    }

    dev_info(&pdev->dev, "Shared memory driver initialized\n");
    return 0;
}

static int shared_mem_remove(struct platform_device *pdev)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    //iounmap(mem_start);
    return 0;
}

static const struct of_device_id shared_mem_of_match[] = {
    { .compatible = "skyfend,shared-mem", },
    {},
};
MODULE_DEVICE_TABLE(of, shared_mem_of_match);

static struct platform_driver shared_mem_driver = {
    .probe = shared_mem_probe,
    .remove = shared_mem_remove,
    .driver = {
        .name = DEVICE_NAME,
        .of_match_table = shared_mem_of_match,
        .owner = THIS_MODULE,
    },
};

static int __init shared_mem_init(void)
{
    return platform_driver_register(&shared_mem_driver);
}

static void __exit shared_mem_exit(void)
{
    platform_driver_unregister(&shared_mem_driver);
}

module_init(shared_mem_init);
module_exit(shared_mem_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chenyili");
MODULE_DESCRIPTION("Shared Memory Driver");		
		
