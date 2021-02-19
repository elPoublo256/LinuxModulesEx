#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/string.h>
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);



static int major = 240;
static int minor = 0;
//static struct semaphore *sem_write; for linux 2.6.2

#define SUCCESS 0
#define DEVICE_NAME "solution_node"	/* Dev name as it appears in /proc/devices   */
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

static int __init init_chardev(void)
{
	dev_t devno = MKDEV(major, minor);
	int res_alloc_dev;
	res_alloc_dev = register_chrdev_region(&devno, 1,DEVICE_NAME);
	if(res_alloc_dev)
	{
		printk(KERN_ERR "error alloc chardev region %d\n",res_alloc_dev);
		return -1;
	}
	is_dev_registerd = true;
	printk(KERN_INFO "allocated chardev major=%d minor=%d\n", major, minor);
	dev = cdev_alloc();
	if(!dev)
	{
		printk(KERN_ERR"error cdev_alloc\n"); return -1;
				}
	dev->ops=&fops;
	int res_add;
	if(cdev_add(dev, devno, 1) < 0)
	{
		printk(KERN_ERR "error cdev_add \n");
		return -1;
	}	
	is_dev_add = true;
	printk(KERN_INFO "register chardev SUCCSES\n");
	return 0;
}



static void __exit cleanup_chardev(void)
{
	printk(KERN_INFO "clean chardev\n");
	
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

module_init(init_chardev);
module_exit(cleanup_chardev);

static size_t size_buf = 0;
static int num_open = 0;

static int device_open(struct inode *n, struct file *f)
{
	printk(KERN_INFO "chardev open\n");
	num_open++;
	return 0;
}
static int device_release(struct inode *n, struct file *f)
{
	printk(KERN_INFO "chardev relese\n");
	return 0;
}

static bool is_eof = false;
static ssize_t device_read(struct file *f, __user char *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "chardev read\n");
	char *buf_print = kmalloc(11, GFP_KERNEL);
	if(!buf_print)
	{
		printk(KERN_ERR "error kalloc in  device_read\n");
		kfree(buf_print);
		return -ENOMEM;
	}
	ssize_t size_mess = sprintf(buf_print, "%d %d\n",num_open, size_buf);
	if(size_mess > 0)
	{
		if(is_eof)
		{
			is_eof = false;
			return 0;
		}
			
		if(copy_to_user(buf, buf_print, size_mess) == 0)
		{
			kfree(buf_print);
			is_eof = true;
			return size_mess;
		}
		printk(KERN_ERR "error copy_to_user\n");

		return -EIO;
	}
	else
	{
		printk(KERN_ERR "error sprintf\n");
		return -EIO;
	}


}
static ssize_t device_write(struct file *f, const char __user *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "device_write\n");
	size_buf += s;
	return s;
}
