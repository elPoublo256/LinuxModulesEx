#obj-m +=  checker.o
obj-m +=  solution.o

export KROOT=/lib/modules/$(shell uname -r)/build

allofit:  modules
modules:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules
#modules_install:
#	@$(MAKE) -C $(KROOT) M=$(PWD) modules_install
clean:
	@$(MAKE) -C $(KROOT) M=$(PWD) clean

#clean: kernel_clean
#	rm -rf   Module.symvers modules.order
