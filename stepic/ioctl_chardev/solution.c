#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static loff_t device_lseek(struct file *, loff_t, int);
static int my_ioctl(struct inode * node, struct file * fl, unsigned int command, unsigned long arg);  
static long my_umlocked_ioctl( struct file *filp,
                 unsigned int cmd, unsigned long arg);

static int major = 240;
static int minor = 0;
static struct mutex __m;
static struct mutex* mut = &__m;
//static struct semaphore *sem_write; for linux 2.6.2

#define SUCCESS 0
#define DEVICE_NAME "solution_node"	/* Dev name as it appears in /proc/devices   */
#define SIZE_BUF 255

#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

/* 
 * Global variables are declared as static, so are global within the file. 
 */


static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = my_ioctl,
	//.ioctl = my_ioctl,
	.compat_ioctl = my_umlocked_ioctl,
	.open = device_open,
	.release = device_release,
	//.llseek = device_lseek

};
static struct cdev *dev = NULL;

static bool is_dev_registerd = false;
static bool is_dev_add = false;
static size_t size_buf;


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
	printk(KERN_INFO "kernel_mooc allocated chardev major=%d minor=%d\n", major, minor);
	dev = cdev_alloc();
	if(!dev)
	{
		printk(KERN_ERR"kernel_mooc error cdev_alloc\n"); return -1;
				}
	dev->ops=&fops;
	int res_add;
	if(cdev_add(dev, devno, 1) < 0)
	{
		printk(KERN_ERR "error cdev_add \n");
		return -1;
	}	
	is_dev_add = true;
	printk(KERN_INFO "kernel_mooc register chardev SUCCSES\n");
	return 0;
}



static void __exit cleanup_chardev(void)
{
	printk(KERN_INFO "kernel_mooc clean chardev\n");
	
	if(is_dev_add)
	{
		printk(KERN_INFO "kernel_mooc remove chardev as device from system\n");
		cdev_del(dev);
	}

	if(is_dev_registerd)
	{
		dev_t devno	= MKDEV(major, minor);
		printk(KERN_INFO "kernel_mooc unregister major=%d minor=%d\n",major,minor); 
		unregister_chrdev_region(devno,1);
	}	
}

module_init(init_chardev);
module_exit(cleanup_chardev);



static int device_open(struct inode *n, struct file *f)
{
	printk(KERN_INFO "kernel_mooc chardev open\n");
	return 0;
}
static int device_release(struct inode *n, struct file *f)
{
	printk(KERN_INFO "kernel_mooc chardev relese\n");
	return 0;
}
static ssize_t device_read(struct file *f, __user char *buf, size_t s, loff_t *l)
{
		printk(KERN_INFO "kernel_mooc read eof\n");
		return 0;

}
static ssize_t device_write(struct file *f, const char __user *buf, size_t s, loff_t *l)
{

	return 0;
}


static loff_t device_lseek(struct file * f, loff_t ofset, int origin)
{
	return 0;
}

static size_t num_len = 0;
static long num_content = 0;
static size_t M4444 = 0;
static size_t N5555 = 0;
static long my_umlocked_ioctl( struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	printk(KERN_INFO, "kernel_mooc unloced_ioctl command=%d\n",cmd);
	char *arg_str = (char*)arg;
	size_t len_arg;
	switch(cmd)
	{
		case SUM_LENGTH:
			printk(KERN_INFO "SUM_LEN\n");
			len_arg = strlen(arg_str);
			if(len_arg < 20)
			{
				num_len += len_arg;
				return num_len;
			}
			return 0;

		case SUM_CONTENT:
			printk(KERN_INFO "SUM_LEN\n");
		       	
                        if(len_arg < 20)
                        {
                                if(kstrtol(arg_str,10,len_arg))
				{
					return 0;
				}
				num_content += len_arg;
				return num_content;
  
                        }
                        return 0;

		case 4444:
			if(kstrtol(arg_str,10,len_arg))
                                {
                                        return 0;
                                }

	}


	return 0;
}

static int my_ioctl(struct inode * node, struct file * fl, unsigned int command, unsigned long arg)
{
	printk(KERN_INFO, "kernel_mooc ioctl command=%d\n",command);
	return 0;
}
