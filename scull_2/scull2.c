#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("El Puablo");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.02");

static int major = 0;
static int minor = 0;
static int num_devs = 1;
static int qset=1000;
static int quantum = 1000;

struct sc_qset { void **data; struct sc_qset* next;};

struct sc_dev
{
	struct cdev dev;
	int size_quant;
	int num_qset;
	unsigned int size_data;
	struct sc_qset* data;

};

//ERROR COMPILE static struct file_operation scul_op = { .owner = THIS_MODULE};

static int __init scull_2_init(void)
{
	int result;
	dev_t dev = 0;
	result = alloc_chrdev_region(&dev,minor,num_devs, "scull2");
	major = MAJOR(dev);
	if(result == 0){printk("scull2 create devici major=%d",major);}
	else{printk("error create scull2");}

	return result;
}


static void __exit scull_2_exit(void)
{
	dev_t dev;
	dev = MKDEV(major,minor);
	printk("unregister dev maj=%d, min=%d",major,minor);
	//unregister_
}

module_init(scull_2_init);
module_exit(scull_2_exit);
