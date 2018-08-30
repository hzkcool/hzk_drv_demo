#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>

#include <linux/poll.h>
#include <linux/cdev.h>

int major;

static struct cdev hello_dev;
static struct class *cls;

static int hello_open (struct inode *inode, struct file *file)
{
    printk("open\n");
    return 0;
}

static struct file_operations hello_fops = {
    .owner    =  THIS_MODULE,
    .open     =  hello_open,
};

static int hello_init(void)
{
    dev_t  device_id;

    if(major){
        device_id = MKDEV(major,0);
        register_chrdev_region(device_id, 2, "hello_device");
    } else {
        alloc_chrdev_region(&device_id, 0, 2, "hello_device");
        major = MAJOR(device_id);
    }

    cdev_init(&hello_dev, &hello_fops);
    cdev_add(&hello_dev, device_id, 2);

    cls = class_create(THIS_MODULE, "hello_device");

    class_device_create(cls, NULL, MKDEV(major, 0), NULL, "hello_device0");
    class_device_create(cls, NULL, MKDEV(major, 1), NULL, "hello_device1");
    class_device_create(cls, NULL, MKDEV(major, 2), NULL, "hello_device2");
    return 0;
}

static void hello_exit(void)
{
    class_device_destroy(cls, MKDEV(major, 0));
    class_device_destroy(cls, MKDEV(major, 1));
    class_device_destroy(cls, MKDEV(major, 2));
    class_destroy(cls);

    cdev_del(&hello_dev);
    unregister_chrdev_region(MKDEV(major, 0), 2);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");