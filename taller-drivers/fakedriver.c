#include "fakeserialhelper.h"
#include <linux/kfifo.h>
#include <linux/semaphore.h>

#define DEVICE_NAME "fakeserial"
#define FAKESERIAL_FIFO_SIZE PAGE_SIZE

static ssize_t fakeserial_read(struct file*,char __user*,size_t size,loff_t*);

struct file_operations fakeserial_fops = {
	.read = fakeserial_read
};

struct miscdevice fakedevice = {
	MISC_DYNAMIC_MINOR,
	DEVICE_NAME,
	&fakeserial_fops
};

static int __init fakeserial_init(void) {
	printk(KERN_DEBUG "fakeserial: Initialized!\n");

	if (misc_register(&fakedevice) < 0) {
		printk(KERN_ALERT "fakeserial: Failed while registering device!\n");
		return 1;
	}

	printk(KERN_DEBUG "fakeserial: %d minor number asigned!\n", fakedevice.minor);
	return 0;
}

static void __exit fakeserial_exit(void) {
	misc_deregister(&fakedevice);
	printk(KERN_DEBUG "fakeserial: device unregistered\n");
}

static ssize_t fakeserial_read(struct file* fp,
                               char __user* user,
                               size_t size,
                               loff_t* offset) {
	return 0;
}


module_init(fakeserial_init);
module_exit(fakeserial_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alumno");
MODULE_DESCRIPTION("Driver del dispositivo serial ficticio");
