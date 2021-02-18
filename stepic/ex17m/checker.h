#pragma once
#include <linux/types.h>
#define CHECKER_MACRO KERN_WARNING


void call_me(const char* message);

int array_sum(short *arr, size_t n);

ssize_t generate_output(int sum, short *arr, size_t size, char *buf);

//return size of bufer wich should allocate
ssize_t get_void_size(void);

///for checking ... wtf?
void submit_void_ptr(void *p);


ssize_t get_int_array_size(void);

void submit_int_array_ptr(int *p);

void submit_struct_ptr(struct device *p);

void checker_kfree(void *p);
