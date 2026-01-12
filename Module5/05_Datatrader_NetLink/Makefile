obj-m += datatrader_netlink.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: module test_prog

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test_prog: test_netlink.c
	gcc test_netlink.c -o test_netlink 

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f test_netlink

load:
	sudo insmod datatrader_netlink.ko

unload:
	sudo rmmod datatrader_netlink

check:
	dmesg | tail -20

test: module test_prog
	bash ./test.sh