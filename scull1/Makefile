obj-m += skul1.o
all:
	make -C /lib/modules/4.15.0-126-generic/build M=$(PWD) modules
##	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/4.15.0-126-generic/build M=$(PWD) clean
##	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

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

