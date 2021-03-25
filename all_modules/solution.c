#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/list_sort.h>
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
	printk(KERN_INFO "read_show\n");
	return sprintf(buf,"%d\n", count_read);
}


static struct module* module_list;

static ssize_t my_sys_show(struct kobject *kobj,
	       	struct kobj_attribute *attr,
      		char *buf)
{
	printk(KERN_INFO "my_sys_show\n");
	ssize_t size = 0;
	struct list_head *end = module_list->list.next;
	struct list_head* next = NULL;
	struct module* mod;
	int i=0;
	char* buf_ptr = buf;
	//return sprintf(buf,"%d\n", sum);
	list_for_each(next, &module_list->list)
        {

                mod = list_entry(next, struct module, list);
		
		ssize_t s_size = sprintf(buf_ptr,"%s\n",mod->name);
		if(s_size >= 0)
		{
			size += s_size;
			buf_ptr += s_size;
		}
		else
		{
			printk(KERN_ERR "error read kobj\n");
			return s_size;
		}
                printk(KERN_INFO "mod name:{%s} i=%d", mod->name, i);
				i++;
				if(i > 100)
				{
					return;
				}
				if(next == end->prev)
					break;
        }
	return size;
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

static int module_compate(void* priv_dat, struct list_head *la, struct list_head *lb)
{
	struct module* mod_a = list_entry(la, struct module, list);
	struct module* mod_b = list_entry(lb, struct module, list);
	return strcmp(mod_a->name, mod_b->name); 
}

static inline void all_modules(void)
{
	int i =0;
	struct module* mod = THIS_MODULE;
	struct module* this_mod = THIS_MODULE;
	struct list_head* next = NULL;
	struct list_head* end = mod->list.prev;
	list_for_each(next,&this_mod->list)
	{
		mod = list_entry(next, struct module, list);
		struct module* new_mod = kmalloc(sizeof(struct module),GFP_KERNEL);
		strcpy((char*)new_mod->name, mod->name);
		INIT_LIST_HEAD(&new_mod->list);
		printk(KERN_INFO "add mod name:{%s} in list", new_mod->name);
		if(!i)
		{
			strcpy(module_list->name, mod->name);
		}
		else
		{

		list_add(&new_mod->list,&module_list->list);

				if(i > 100)
				{
					printk(KERN_ERR "AAAAAAAAA i = 100\n");
					return;
				}
				if(next==end)
				{
					break;
				}
		}
		i++;

	}
}

static void print_list_mod(struct module* mod)
{
	printk(KERN_INFO "--------------print_list_mod--------------\n");
	struct list_head *next;
	struct list_head *end = mod->list.next;
	int i = 0;
	list_for_each(next, &mod->list)
        {

                mod = list_entry(next, struct module, list);
                printk(KERN_INFO "mod name:{%s} i=%d", mod->name, i);
				i++;
				if(i > 100)
				{
					return;
				}
				if(next == end->prev)
					break;
        }
}

int __init init_kobj(void)
{
	printk(KERN_INFO "init module\n");


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

	module_list = kmalloc(sizeof(struct module), GFP_KERNEL);
	INIT_LIST_HEAD(&module_list->list);




	all_modules();
	list_sort(NULL, &module_list->list, module_compate);

	print_list_mod(module_list);
	return 0;// retval;
}


static void __exit cleanup_kobj(void)
{
	kobject_put(kobj);
	kfree(module_list);
	printk(KERN_INFO "cleanup_kobj\n");
}

module_init(init_kobj);
module_exit(cleanup_kobj);

MODULE_LICENSE("GPL"); //no work sysfs without it (Unknown symbol kernel_kobj)
