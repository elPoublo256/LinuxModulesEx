#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
//#include "page.h"
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);



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

static int __init init_chardev(void)
{
	dev_t devno;
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
	dev->ops=&fops;
	int res_add;
	res_add = cdev_add(dev, devno, 1);
	if(res_add < 0)
	{
		printk(KERN_ERR "error cdev_add %d\n", res_add);
		return -1;
	}	
	is_dev_add = true;
	printk(KERN_INFO "register chardev SUCCSES\n");
	return 0;
}



static char* dev_buf = NULL;
static void __exit cleanup_chardev(void)
{
	if(dev_buf)
	{
		kfree(dev_buf);
	}
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
static bool buzy = false;

static int device_open(struct inode *n, struct file *f)
{
	printk(KERN_INFO "chardev open\n");
	if(buzy)
		return -EBUSY;

	buzy = true;
	return 0;
}
static int device_release(struct inode *n, struct file *f)
{
	printk(KERN_INFO "chardev relese\n");
	buzy = false;
	return 0;
}
inline size_t ready_read(loff_t *l)
{
	return size_buf - *l;
}
static ssize_t device_read(struct file *f, char *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "chardev read\n");
	if(*l >= size_buf)
	{
		printk(KERN_INFO "user file pos biger then size dev_buf\n");
		return -EIO;//TODO END OF FILE
	}
	if(s)
	{
	//i know that copy_to_user call acces_ok and if it return true cal __copy_to_user
	//but i get this knowleg after i writen this code and i too lezy change this.	
		if(access_ok(VERIFY_WRITE,buf,s))
			{
					size_t s_read = size_buf - *l;
					if(s <= s_read)
					{
						printk(KERN_INFO "user read full request s=%lu\n",s);
						copy_to_user(buf, dev_buf + *l, s);
						return s;	
					}
					else if(s > s_read)
					{
						printk(KERN_INFO "user try read too much\n");
						copy_to_user(buf, dev_buf + *l, s_read);
					       return s_read;	
					}
			}
		else
		{
			printk(KERN_INFO "error acces_ok\n");
			return 0;
		}

	}
	return 0;
}

static inline ssize_t copy_from_user_check(const char* __user ubuf, size_t s, size_t dist)
{
	if(dist + s > SIZE_BUF)
	{
		printk(KERN_INFO "to much %lu data for dev_buf, resize\n", s);
		s = s - (SIZE_BUF - dist);
	}

	printk(KERN_INFO "write to dev %lub\n", s);
	if(copy_from_user(dev_buf + dist, ubuf,s))
	{
		printk(KERN_ERR "error access write dev\n");
		return -EIO;
	}
	else
	{
		size_buf = dist + s;
		return s;
	}		
}
static inline ssize_t write_device_success(const char* __user ubuf, size_t s, loff_t *l)
{
	if(*l == size_buf)
	{
		return copy_from_user_check(ubuf,s,size_buf);
	}

	else if(*l < size_buf)
	{
		size_t dist = *l;
		size_t s_wr = (size_buf - *l);
		return copy_from_user_check(ubuf, s_wr, dist);
	}
	else
	{
		printk(KERN_INFO " pos > qurent size of writen bufer\n");
		return -EIO;
	}

	
}

static ssize_t device_write(struct file *f, const char __user *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "chardev write\n");
	if(dev_buf == NULL)
	{
		printk(KERN_INFO "there are no data\n");
		dev_buf = kmalloc(SIZE_BUF, GFP_KERNEL);
	}
	if(*l >= SIZE_BUF)
        {
                printk(KERN_INFO "user file pos biger then size dev_buf\n");
                return -EIO;//TODO END OF FILE
        }
	if(dev_buf)
	{
		if(access_ok(VERIFY_READ,buf,s))
				{
					return write_device_success(buf,s,l);
				}
		else
		{
			printk(KERN_INFO "error access user buf for read");
		       return -EIO;
		}	       
		 	
	}
	else
	{
		printk(KERN_ERR "error allocate dev_buf\n");
	}
	return 0;
}
