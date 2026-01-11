obj-m += datatrader_chardev.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	sudo insmod datatrader.ko

unload:
	sudo rmmod datatrader

check:
	dmesg | tail -20

test:
	bash ./test.sh