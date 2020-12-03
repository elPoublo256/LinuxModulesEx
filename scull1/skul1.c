#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


//#include <linux/types.h> //determine dev_t
#include <linux/kdev_t.h> //determine macro MAJOR(dev_t) MINOR(dev_t) MKDEV(int,int)

#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("El Puablo");
MODULE_DESCRIPTION("A simple example linux device as skul.");
MODULE_VERSION("0.02");

dev_t device;

static unsigned int count = 1;

static int __init skul1_init(void)
{
//	dev_t first = 0;
//	dev_t dev;
	const char* name_dev = "skul1";
	unsigned int first_minor = 1;
	 
	int err = alloc_chrdev_region(&device, first_minor, count, name_dev);
	if(err == 0)
	{
		int major_v, minor_v;
		major_v = MAJOR(device);
		minor_v = MINOR(device);
//		printk("create device maj_v=%d, min_v=%d", major_v, minor_v);
		return 0;
	}
	else
	{
		printk("Error register device - %d",err);
		return err;
	}


 return 0;	
}
static void __exit skul1_exit(void)
{
	printk("unregister device");
	unregister_chrdev_region(device,count);
}


module_init(skul1_init);
module_exit(skul1_exit);
