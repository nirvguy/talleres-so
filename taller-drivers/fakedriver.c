#include "fakeserialhelper.h"
#include <linux/kfifo.h>
#include <linux/semaphore.h>
#include <linux/types.h>

#define DEVICE_NAME "fakeserial"
#define FAKESERIAL_FIFO_SIZE PAGE_SIZE

#define KRAND_MAX                  2147483648UL
#define KRAND_A                    1103515245UL
#define KRAND_C                    12345UL

u64 next_rand_nr;

static ssize_t fakeserial_read(struct file*,char __user*,size_t size,loff_t*);

struct file_operations fakeserial_fops = {
	.read = fakeserial_read
};

struct miscdevice fakedevice = {
	MISC_DYNAMIC_MINOR,
	DEVICE_NAME,
	&fakeserial_fops
};

u64 get_rand_number(u64 n) {
	return (KRAND_A * n + KRAND_C) % KRAND_MAX;
}

static int __init fakeserial_init(void) {
	printk(KERN_DEBUG "fakeserial: Initialized!\n");

	if (misc_register(&fakedevice) < 0) {
		printk(KERN_ALERT "fakeserial: Failed while registering device!\n");
		return 1;
	}

	printk(KERN_DEBUG "fakeserial: %d minor number asigned!\n", fakedevice.minor);
	return 0;

	next_rand_nr = 0;
}

static void __exit fakeserial_exit(void) {
	misc_deregister(&fakedevice);
	printk(KERN_DEBUG "fakeserial: device unregistered\n");
}

static ssize_t fakeserial_read(struct file* fp,
                               char __user* user,
                               size_t size,
                               loff_t* offset) {
	size_t k = size / sizeof(int);
	unsigned int rest = size % sizeof(int);
	size_t i = 0;
	unsigned int prev_rand_nr = 0;

	for (i=0; i<k; i++, user += sizeof(int)) {
		prev_rand_nr = next_rand_nr;
		copy_to_user(user, (char*) &prev_rand_nr, sizeof(int));
		next_rand_nr = get_rand_number(next_rand_nr);
	}

	if (rest > 0) {
		prev_rand_nr = next_rand_nr;
		copy_to_user(user, (char*) &prev_rand_nr, rest);
		next_rand_nr = get_rand_number(next_rand_nr);
	}

	return size;
}


module_init(fakeserial_init);
module_exit(fakeserial_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alumno");
MODULE_DESCRIPTION("Driver del dispositivo serial ficticio");
