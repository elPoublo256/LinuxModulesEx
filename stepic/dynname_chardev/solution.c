#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/string.h>
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static char* node_name = "my_node";
module_param(node_name, charp, S_IRUGO);

static int major = 0;
static int minor = 0;

#define SUCCESS 0
#define DEVICE_NAME "mychardev"	/* Dev name as it appears in /proc/devices   */
#define SIZE_BUF 1028
/* 
 * Global variables are declared as static, so are global within the file. 
 */


static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static struct cdev *dev = NULL;

static bool is_dev_registerd = false;
static bool is_dev_add = false;

static struct class *my_class = NULL;
static struct device *my_device = NULL;


static int init_my_class(dev_t devno)
{
	printk("start create class device maj=%d min=%d",MAJOR(devno), MINOR(devno));
	my_class = class_create(THIS_MODULE, node_name);
	if(my_class)
	{
		my_device = device_create(my_class, NULL, devno, "%s", node_name);
		if(my_device)
		{
			printk(KERN_INFO "create device %s SUCSESS\n", node_name); 
			return 0;
		}
		printk(KERN_ERR "error create device\n");
		return -1;

	}
	printk(KERN_ERR "error create class\n");
	return -1;
}

static void clear_my_class(dev_t devno)
{

}

inline void my_clean(void)
{
	
	if(is_dev_add)
	{
		printk(KERN_INFO "remove chardev as device from system\n");
		cdev_del(dev);
	}

	if(is_dev_registerd)
	{
		dev_t devno	= MKDEV(major, minor);
		printk(KERN_INFO "unregister major=%d minor=%d\n",major,minor); 
		unregister_chrdev_region(devno,1);
	}	
}

static int __init init_chardev(void)
{
	dev_t devno;
	//init_MUTEX(sem_write); for linux 2.6.2
	int res_alloc_dev;
	res_alloc_dev = alloc_chrdev_region(&devno, minor,1,DEVICE_NAME);
	if(res_alloc_dev)
	{
		printk(KERN_ERR "error alloc chardev region %d\n",res_alloc_dev);
		return -1;
	}
	major = MAJOR(devno);
	is_dev_registerd = true;
	minor = MINOR(devno);
	printk(KERN_INFO "allocated chardev major=%d minor=%d\n", major, minor);
	dev = cdev_alloc();
	if(!dev)
	{
		printk(KERN_ERR"error cdev_alloc\n"); return -1;
				}
	//dev->ops=&fops;
	cdev_init(dev,&fops);
	int res_add;
	res_add = cdev_add(dev, devno, 1);
	if(res_add < 0)
	{
		printk(KERN_ERR "error cdev_add %d\n", res_add);
		return -1;
	}	
	is_dev_add = true;
	printk(KERN_INFO "register chardev SUCCSES\n");
	int res_class = init_my_class(devno);
	if(res_class)
	{
		printk(KERN_ERR "error create class");
		my_clean();
		return res_class;
	}

	return 0;
}



static void __exit cleanup_chardev(void)
{
	printk(KERN_INFO " __exit cleanup_chardev\n");
	if(my_class)
	{
		device_destroy(my_class, MKDEV(major,minor));
		class_unregister(my_class);
		class_destroy(my_class);
	}
	else
	{
		printk(KERN_ERR "cant delete class\n");
	}
	my_clean();
}

module_init(init_chardev);
module_exit(cleanup_chardev);

static size_t size_buf = 0;
static bool buzy = false;

static int device_open(struct inode *n, struct file *f)
{
	printk(KERN_INFO "chardev open\n");
	return 0;
}
static int device_release(struct inode *n, struct file *f)
{
	return 0;
}
inline size_t ready_read(loff_t *l)
{
	return size_buf - *l;
}
static bool is_end = false;
static ssize_t device_read(struct file *f, __user char *buf, size_t s, loff_t *l)
{
	if(is_end)
	{
		is_end = false;
		return 0;
	}

	is_end = true;

	printk(KERN_INFO "chardev read\n");
	char* buf_p = kmalloc(5,GFP_KERNEL);
	if(buf_p)
	{
		size_t l = sprintf(buf_p,"%d\n",major);
	 
		if(l)
		{
		       if(copy_to_user(buf,buf_p,l) == 0)
		       {
			       kfree(buf_p);
			       
			       return l;
		       }
		       else
		       {
			       kfree(buf_p);
			       printk(KERN_ERR "error copy to user\n");
			       return -EIO;
		       }
		       kfree(buf_p);
			printk("error sprintf\n");
			return -EIO;

		}

	}
	else
	{
		kfree(buf_p);
		return -ENOMEM;
	}

	return 0;
}

static ssize_t device_write(struct file *f, const char __user *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "chardev write\n");
	return 0;
}
MODULE_LICENSE("GPL");
