#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h> ///for using kallok
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("El Puablo");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.06");

static int major = 0;
static int minor = 0;
static int num_devs = 1;
//static int qset=1000;
//static int quantum = 1000;
#define SIZE_SCULL_DATA 4000
struct sc_qset { void **data; struct sc_qset* next;};

struct sc_dev
{
	struct cdev* dev;
//	int size_quant;
//	int num_qset;
//	unsigned int size_data;
//	struct sc_qset* data;
	char* data;
};

struct sc_dev* sc_dev_ptr;


static int sc_trim(struct sc_dev *sc)
{
	printk(KERN_INFO "run sc_trim");
	printk(KERN_INFO "free data from scull\n");
	kfree(sc->data);
	return 0;
}

static int delete_sc(struct sc_dev *sc)
{
int res = 0;
if(sc->dev)
{
	cdev_del(sc->dev);
}
return res;
}

static int scull_open(struct inode *inode, struct file *filp)
{
    return 0;
}

/* close operation */
static int scull_release(struct inode *inode, struct file *flip)
{
    return 0;
}
/* read function */
static ssize_t scull_read(struct file *flip, char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

/* write function */
static ssize_t scull_write(struct file *flip, const char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

/* File operations structure */
static struct file_operations scull_fops = {
    .owner = THIS_MODULE, /* used to prevent unload while operations are used */
    .open = scull_open,
    .release = scull_release,
    .read = scull_read,
    .write = scull_write,
};

static int setup_sc(struct sc_dev *sc)
{

	sc->data = kmalloc(SIZE_SCULL_DATA, GFP_KERNEL);
	if(sc->data)
	{
	int devno;
	int err = 0;
	devno = MKDEV(major, minor);
	printk(KERN_INFO"create devno=%d\n",devno);
	printk("register sc as char-dev\n");
	sc->dev = cdev_alloc();
	sc->dev->ops = &scull_fops;
	// other whay: cdev_init(sc->dev,&fp);
	err = cdev_add(sc->dev,devno,1);
	if(err){printk(KERN_NOTICE "error cdev_add %d\n",err);}
	return err;}
	else
	{
		printk(KERN_NOTICE "error kmalloc in setup_sc\n");
		return -1;
	}
	
}
static int __init scull_2_init(void)
{
	printk(KERN_INFO"hellow\n");
	int result;
	dev_t dev = 0;
	result = alloc_chrdev_region(&dev,minor,num_devs, "scull2");
	major = MAJOR(dev);
	if(result == 0){printk("scull2 create devici major=%d\n",major);}
	else{printk("error create scull2\n"); return result;}
	printk("alloc sc_dev\n");
	sc_dev_ptr = kmalloc(num_devs * sizeof(struct sc_dev), GFP_KERNEL);
	if(!sc_dev_ptr)
	{
		result = -ENOMEM;
		goto fail;
	}
	else 
	{
		printk("alloc sc_dev - DONE\n");
		memset(sc_dev_ptr, 0, sizeof(struct sc_dev));
		setup_sc(sc_dev_ptr);
		result = setup_sc(sc_dev_ptr);
	}

	return result;
fail:
	delete_sc(sc_dev_ptr);
	return result;
	
}


static void __exit scull_2_exit(void)
{
	printk(KERN_INFO"goodbuy\n");
	dev_t dev;
	dev = MKDEV(major,minor);
	printk(KERN_INFO"unregister dev maj=%d, min=%d\n",major,minor);
	unregister_chrdev_region(dev,1);
}

module_init(scull_2_init);
module_exit(scull_2_exit);
