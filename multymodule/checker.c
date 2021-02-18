#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h> // for kmallock
#include <linux/random.h>

#include "checker.h"
void call_me(const char* message)
{
	printk(KERN_INFO "call_me:%s",message);
}
EXPORT_SYMBOL(call_me);

int array_sum(short *arr, size_t n)
{
	int res = 0;
	int i;
	for(i = 0; i < n; i++)
	{
		res = res + arr[i];
	}
	return res;
}
EXPORT_SYMBOL(array_sum);

ssize_t generate_output(int sum, short *arr, size_t size, char *buf)
{
	ssize_t real_len = 0;
	real_len += sprintf(buf,"<%d>",sum);
	size_t i;
	for(i = 0; i < size; i++)
	{
		real_len += sprintf(buf + real_len,"<%d>",arr[i]);
	}
	return real_len;
}
EXPORT_SYMBOL(generate_output);

static int __init my_init(void)
{
	printk(KERN_INFO "INIT CHECKER");
	return 0;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "Bye from CHECKER");
}

ssize_t get_void_size(){return 583321;}
EXPORT_SYMBOL(get_void_size);


ssize_t get_int_array_size(void)
{
	return 5;
}
EXPORT_SYMBOL(get_int_array_size);

void submit_void_ptr(void* p)
{
	size_t s_ptr = ksize(p);
	if(s_ptr==get_void_size())
	{
		printk(KERN_INFO " VOID SIZE OK\n");
	}
	else
	{
		printk(KERN_ERR "VOID SIZE ERROR S=%lu\n",s_ptr);
	}
}
EXPORT_SYMBOL(submit_void_ptr);


void submit_int_array_ptr(int *p)
{
	size_t s_ptr = ksize(p);
        if(s_ptr==get_int_array_size() * sizeof(int))
        {
                printk(KERN_INFO " INT SIZE OK\n");
        }
        else
        {
                printk(KERN_ERR "INT SIZE ERROR S=%lu\n",s_ptr);
        }

}
EXPORT_SYMBOL(submit_int_array_ptr);

void submit_struct_ptr(struct device *p)
{
	size_t s_ptr = ksize(p);
        if(s_ptr==sizeof(struct device))
        {
                printk(KERN_INFO " DEV SIZE OK\n");
        }
        else
        {
                printk(KERN_ERR "DEV SIZE ERROR S=%lu\n",s_ptr);
        }
}
EXPORT_SYMBOL(submit_struct_ptr);

void checker_kfree(void* p)
{
	kfree(p);
	printk(KERN_INFO "check free\n");
}
EXPORT_SYMBOL(checker_kfree);


module_init(my_init);
module_exit(my_exit);
