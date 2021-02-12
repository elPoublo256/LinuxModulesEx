obj-m += chardev.o
ccflags-y := -std=gnu11 -Wno-declaration-after-statement
#FLAGS=-std=gnu99 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc test_chardev.c
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm a.out

#ifneq ($(KERNELRELESE),)
#	obj-m += printparam.o
#else
#	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#	PWD := $(shell pwd)
#all:
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
#endif
#clean:
#	make -C $(KERNELDIR) M=$(PWD) clean

