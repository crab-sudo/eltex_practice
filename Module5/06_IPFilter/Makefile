obj-m += ipfilter.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	sudo insmod ipfilter.ko

unload:
	sudo rmmod ipfilter

check:
	dmesg | tail -10

test:
	bash ./test.sh