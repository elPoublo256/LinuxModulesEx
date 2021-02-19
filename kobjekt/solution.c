#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int count_read = 0;
static int my_sys = 0;

static int a=0;
static int b=0;
static int c[5] = {0,0,0,0,0};
module_param(a,int,0);
module_param(b,int,0);
static int size_c = 5;
module_param_array(c,int,&size_c,0);

static int sum = 0;

static ssize_t read_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf,"%d\n", count_read);
}

static ssize_t my_sys_show(struct kobject *kobj,
	       	struct kobj_attribute *attr,
      		char *buf)
{

	return sprintf(buf,"%d\n", sum);
}


static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr,
      const char *buf, size_t count)
{
	printk("my_sys_store\n");
	return count;
}
/*
void kobj_relese(struct kobject *obj)
{

}
*/

static struct kobj_attribute atr_my_sys = __ATTR(my_sys,
	       					0664,
					       	my_sys_show,
					       	my_sys_store);


static struct attribute *attrs[] = {
	&atr_my_sys.attr,
	NULL,
};


static struct attribute_group attr_gr = {
	.attrs = attrs,
};


static struct kobject *kobj;

static inline void print_params(void)
{
	printk(KERN_INFO "a=%d b=%d c={\n",a,b);
	int i;
	for(i = 0; i < 5; i++)
	{
		printk(KERN_INFO "%d\n",c[i]);
	}
	printk(KERN_INFO "}\n");
}	


int __init init_kobj(void)
{
	sum = a + b;
	int i;
	for(i = 0; i < 5; i++)
	{
		sum += c[i];
	}

	//printk(KERN_INFO "init_kobj\n");
	//print_params();
	kobj = kobject_create_and_add("my_kobject", kernel_kobj);
	if(!kobj)
	{
		return -ENOMEM;
	}
	int retval = sysfs_create_group(kobj, &attr_gr);
	if(retval)
	{
		kobject_put(kobj);
	}
	 
	return retval;
}


static void __exit cleanup_kobj(void)
{
	kobject_put(kobj);
	//printk(KERN_INFO "cleanup_kobj\n");
}

module_init(init_kobj);
module_exit(cleanup_kobj);

MODULE_LICENSE("GPL"); //no work sysfs without it (Unknown symbol kernel_kobj)
