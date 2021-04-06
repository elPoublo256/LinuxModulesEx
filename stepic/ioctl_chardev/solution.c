#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/ioctl.h>

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static loff_t device_lseek(struct file *, loff_t, int);
static int my_ioctl(struct inode * node, struct file * fl, unsigned int command, unsigned long arg);  

static int major = 240;
static int minor = 0;
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


static size_t num_len = 0;
static long num_content = 0;
static size_t M4444 = 0;
static size_t N5555 = 0;
inline static long sum_length(int size_arg, char* arg)
{
	if(size_arg > 0)
	{
		printk(KERN_INFO "SUM_LENGTH {%s}\n",arg);
		num_len += size_arg;
		return num_len;
	}
	else
	{
		printk(KERN_ERR "error SUM_LENGTH\n");
	}
	return 0;
}

inline static long sum_conent(int size_arg, char* arg)
{
        if(size_arg > 0)
        {
                printk(KERN_INFO "kernel_mooc SUM_CONTENT {%s}\n",arg);
		long content;
		if(kstrtol(arg,10,&content)==0)
		{
			num_content += content;
			return num_content;
		}
		printk(KERN_INFO "kernel_mooc error (kstrtol\n");
		return 0;
                
        }
        else
        {
                printk(KERN_ERR "error SUM_CONTENT\n");
        }
        return 0;
}

inline static long process_4444(int size_arg, char* arg)
{
	if(size_arg > 0)
        {
                printk(KERN_INFO "kernel_mooc   4444 {%s}\n",arg);
                M4444 += size_arg;
                return M4444;
        }
        else
        {
                printk(KERN_INFO "kernel_mooc error 4444\n");
        }
        return 0;

}

inline static long process_5555(int size_arg, char* arg)
{
        if(size_arg > 0)
        {
                printk(KERN_INFO "kernel_mooc 5555 {%s}\n",arg);
                long content;
                if(kstrtol(arg,10,&content)==0)
                {
                        N5555 += content;
                        return N5555;
                }
                printk(KERN_INFO "kernel_mooc error (kstrtol\n");
                return 0;

        }
        else
        {
                printk(KERN_INFO "kernel_mooc error SUM_CONTENT\n");
        }
        return 0;
}


inline static long  process_cmd(int cmd, int size_arg, char* arg)
{
	printk(KERN_INFO "kernel_mooc process_cmd cmd=%d arg={%s}\n", cmd, arg);	
	switch (cmd)
	{
		case SUM_CONTENT:
			printk(KERN_INFO "kernel_mooc  cmd = SUM_CONTENT\n");
			return sum_conent(size_arg, arg);

		case SUM_LENGTH:
			printk(KERN_INFO "kernel_mooc  cmd = SUM_LENGTH\n");
			return sum_length(size_arg, arg);

		case 4444:
			printk(KERN_INFO "kernel_mooc  cmd = 4444\n");
			return process_4444(size_arg, arg);

		case 5555:
			printk(KERN_INFO "kernel_mooc  cmd = 5555\n");
			return process_5555(size_arg, arg);
		default:
			printk(KERN_INFO "kernel_mooc unknown cmd = %d\n",cmd);
			return -ENOTTY;

	}
}

static long my_umlocked_ioctl( struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	size_t len_arg = 0;
	//char* buf = kmalloc(GFP_KERNEL, 20);
	printk(KERN_INFO "kernel_mooc get len\n");
	int ioc_size = _IOC_SIZE(cmd);
	printk(KERN_INFO "kernel_mooc ioc_size=%d\n", ioc_size);
	char* user_ptr = kmalloc(ioc_size, GFP_KERNEL);
	__user char* ptr = (__user char*)arg;
	int size_copy = copy_from_user(user_ptr, ptr, ioc_size);
	printk(KERN_INFO "kernel_mooc copy from user %d b\n", size_copy);
	int size_arg = strlen(user_ptr);
	//if(size_arg > 0)
	//{
	//	if(access_ok(char, user_ptr, size_arg))
	//	{
	//		
	//		printk(KERN_INFO "ACCES OK\n");
			printk(KERN_INFO "kernel_mooc  ioctl arg = {%s}\n",user_ptr);
			long res = process_cmd(cmd, size_arg, user_ptr);
		       	kfree(user_ptr);	
			return res;
	//	}
	//
	//	printk(KERN_INFO "kernel_mooc  NO ACCES\n");
	//	return 0;
	//}
	//printk(KERN_INFO "kernel_mooc ioctl str len=%d\n",size_arg);
	//kfree(buf);

	//return 0;
}
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = my_umlocked_ioctl,
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
static char* kbuf; // for read

static int __init init_chardev(void)
{
	dev_t devno = MKDEV(major, minor);
	int res_alloc_dev;
	kbuf = kmalloc(40, GFP_KERNEL);
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
	kfree(kbuf);
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
static bool is_end = false;
static ssize_t device_read(struct file *f, __user char *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "kernel_mooc device_read\n");
	
            if(is_end)
        {
		printk(KERN_INFO "kernel_mooc dev is reded\n");
                is_end = false;
                return 0;
        }

        is_end = true;

        printk(KERN_INFO "chardev read\n");
        if(kbuf)
        {
                size_t l = sprintf(kbuf,"%ld %ld\n",M4444, N5555);

                if(l)
                {
			printk(KERN_INFO "kernel_mooc user read %s\n",kbuf);
                       if(copy_to_user(buf,kbuf,l) == 0)
                       {
                               

                               return l;
                       }
                       else
                       {
                               
                               printk(KERN_INFO "kernel_mooc error copy to user\n");
                               return -EIO;
                       }
                      
                        printk(KERN_INFO "kernel_mooc error sprintf\n");
                        return -EIO;

                }

        }
        else
        {
                return -ENOMEM;
        }

        return 0;
	
}

static ssize_t device_write(struct file *f, const char __user *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "kernel_mooc write file\n");

	return 0;
}


static loff_t device_lseek(struct file * f, loff_t ofset, int origin)
{
	return 0;
}


static int my_ioctl(struct inode * node, struct file * fl, unsigned int command, unsigned long arg)
{
	printk(KERN_INFO, "kernel_mooc ioctl command=%d\n",command);
	return 0;
}
