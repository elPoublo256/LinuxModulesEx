ifneq ($(KERNELRELESE),)
	obj-m += lkm_ex1.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
clean:
	make -C $(KERNELDIR) M=$(PWD) clean

