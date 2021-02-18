#include <linux/init.h>                                                                                                                                                                                                                            
#include <linux/module.h>                                                                                                                                                                                                                          
#include <linux/kernel.h>                                                                                                                                                                                                                          
#include <linux/random.h>                                                                                                                                                                                                                          
#include <linux/slab.h>                                                                                                                                                                                                                            
#include "checker.h"                                                                                                                                                                                                                               
static int *arr_int_ptr = NULL;
static struct device *dev_ptr = NULL;
static void *void_ptr;

static int __init my_init(void)
{
	ssize_t s_void = get_void_size();
	ssize_t s_arr = get_int_array_size();
	if(s_void > 0)
	{
		void_ptr = kmalloc(s_void, GFP_KERNEL);
		if(ksize(void_ptr) > s_void)
		{
			void_ptr = krealloc(void_ptr,s_void,GFP_KERNEL);
		}	
		submit_void_ptr(void_ptr);
	}
	if(s_arr > 0)
	{
		arr_int_ptr = kmalloc(s_arr * sizeof(int), GFP_KERNEL);
		submit_int_array_ptr(arr_int_ptr);
	}
	dev_ptr = kmalloc(sizeof(struct device), GFP_KERNEL);
	submit_struct_ptr(dev_ptr);
	return 0;

}

static void __exit my_exit(void)
{
	if(void_ptr)
	{
		checker_kfree(void_ptr);
	}

	if(arr_int_ptr)
	{
		checker_kfree(arr_int_ptr);
	}

	if(dev_ptr)
	{
		checker_kfree(dev_ptr);
	}
        //printk(KERN_INFO "Bye from Export Symbol 1 module.");
}

module_init(my_init);
module_exit(my_exit);

