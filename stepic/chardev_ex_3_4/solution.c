#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/mutex.h>
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static loff_t device_lseek(struct file *, loff_t, int);



static int major = 240;
static int minor = 0;
static struct mutex __m;
static struct mutex* mut = &__m;
//static struct semaphore *sem_write; for linux 2.6.2

#define SUCCESS 0
#define DEVICE_NAME "solution_node"	/* Dev name as it appears in /proc/devices   */
#define SIZE_BUF 255
/* 
 * Global variables are declared as static, so are global within the file. 
 */


static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
	.llseek = device_lseek

};
static struct cdev *dev = NULL;

static bool is_dev_registerd = false;
static bool is_dev_add = false;
static size_t size_buf;


static int __init init_chardev(void)
{
	mutex_init(mut);
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

static int num_open = -1;


struct my_file
{
	bool is_first;
	bool is_eof;
	size_t size_writen;
	int num_ses;
	void *buf;
};

static int device_open(struct inode *n, struct file *f)
{
	mutex_lock(mut);
	printk(KERN_INFO "kernel_mooc chardev open\n");
	struct my_file *my_f = kmalloc(sizeof(struct my_file), GFP_KERNEL);
	my_f -> is_first = true;
	my_f -> buf = kmalloc(SIZE_BUF, GFP_KERNEL);
	my_f -> size_writen = 0;
	my_f -> is_eof = false;
	num_open++;
	my_f -> num_ses = num_open;
	f -> private_data = my_f;
	
	mutex_unlock(mut);
	return 0;
}
static int device_release(struct inode *n, struct file *f)
{
	printk(KERN_INFO "kernel_mooc chardev relese\n");
	struct my_file *my_f = (struct my_file*) f->private_data;
	kfree(my_f->buf);
	kfree(my_f);
	return 0;
}

static bool is_eof = false;
static loff_t cure_pos = 0;
static const char endl = '\0';
static inline ssize_t read_number_session(__user char* buf, int n)
{

		char *p_print=kmalloc(5,GFP_KERNEL);
		size_t s_len = sprintf(p_print,"%d",n);

	 	printk(KERN_INFO "kernel_mooc copy to user %s\n", p_print);
		if(copy_to_user(buf, p_print, s_len))
				{
				return -EIO;
				}

		return s_len;
}
static ssize_t device_read(struct file *f, __user char *buf, size_t s, loff_t *l)
{
	printk(KERN_INFO "kernel_mooc read device s=%lu l=%llu\n", s, *l);
	struct my_file *my_f = (struct my_file*) f->private_data;
	if(my_f->is_first)
	{
		 printk(KERN_INFO "kernel_mooc read sid\n");
		my_f->is_first = false;
		return read_number_session(buf, my_f->num_ses);
	}
	printk(KERN_INFO "kernel_mooc read from bufer\n");
	if(my_f->size_writen && *l < my_f->size_writen)
	{
		ssize_t s_read = s + *l <= my_f->size_writen ? s : s + *l - my_f->size_writen;
	       if(copy_to_user(buf, my_f->buf + *l, s_read))
			       {
				       return -EIO;
			       }
	       	*l += s_read;
		printk(KERN_INFO "kernel_mooc read device \n");
		return s_read;	       
	}
	else
	{
		printk(KERN_INFO "kernel_mooc read eof s_buf=%lu, pos=%llu\n", my_f->size_writen, *l);
		return 0;
	}


}
static ssize_t device_write(struct file *f, const char __user *buf, size_t s, loff_t *l)
{

	struct my_file *my_f = (struct my_file*) f->private_data;
	printk(KERN_INFO "kernel_mooc device_write l=%llu s=%lu size_byf=%lu", *l, s,my_f->size_writen);
	if(*l > my_f -> size_writen)
	{
		printk(KERN_INFO "kernel_mooc device_write *l > my_f -> size_write\n");
		return 0;
	}

	if(*l + s > ksize(my_f->buf))
	{
		void* new_buf = kmalloc(*l + s + SIZE_BUF, GFP_KERNEL);
		memcpy(new_buf, my_f->buf, my_f->size_writen);
		kfree(my_f->buf);
		my_f->buf = new_buf;

	}

	if(copy_from_user(my_f->buf + *l, buf, s) == 0)
	{
		*l = *l + s;
		if(*l > my_f->size_writen)
		{
			my_f->size_writen = *l;
		}
		return s;
	}
		
	return 0;
}


static loff_t device_lseek(struct file * f, loff_t ofset, int origin)
{
	printk(KERN_INFO "kernel_mooc lseek of=%llu orig=%d\n",ofset, origin);
	loff_t new_pos;
	switch(origin)
	{
	case SEEK_SET:
		new_pos = ofset;
		printk(KERN_INFO "kernel_mooc lseek SEEK_SET\n");
		break;
	case SEEK_END:
		new_pos = size_buf + ofset; 
		printk(KERN_INFO "kernel_mooc lseek SEEK_END\n");
		break;
	case SEEK_CUR:
		new_pos = cure_pos + ofset;
		printk(KERN_INFO "kernel_mooc lseek SEEK_CUR");
		break;
	default:
		return -EINVAL;

	}
	
	new_pos = new_pos < size_buf ? new_pos : size_buf;
	new_pos >= 0 ? new_pos : 0;
	printk(KERN_INFO "kernel_mooc lseek return %d\n",new_pos);
	f->f_pos = new_pos;
	return new_pos;
}
