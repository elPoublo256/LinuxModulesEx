#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);



static int major = 0;
static int minor = 5;

module_param(major,int,S_IRUGO);

#define SUCCESS 0
#define DEVICE_NAME "chardev"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */

/* 
 * Global variables are declared as static, so are global within the file. 
 */

static int Major;		/* Major number assigned to our device driver */
static int Device_Open = 0;	/* Is device open?  
				 * Used to prevent multiple access to device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

struct cdev *dev;
bool is_dev_registerd;

static int __init init_chardev(void)
{
	is_dev_registerd = false;
	int res = 0;
	int err_add_cdvev;
	int devno;
	devno = MKDEV(major,minor);
	dev = cdev_alloc();
	dev->ops=&fops;
	err_add_cdvev = cdev_add(dev, devno,1);
	return res;
}



static void __exit cleanup_chardev(void)
{
	
}

module_init(init_chardev);
module_exit(cleanup_chardev);


static int device_open(struct inode *n, struct file *f)
{
	return 0;
}
static int device_release(struct inode *n, struct file *f)
{
	return 0;
}
static ssize_t device_read(struct file *f, char *buf, size_t s, loff_t *l)
{
	return 0;
}
static ssize_t device_write(struct file *f, const char *buf, size_t s, loff_t *l)
{
	return 0;
}
