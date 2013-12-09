.PHONY: load

obj-m += opus-driver.o
opus-driver-objs += opus.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


load:
	./load
	sudo rmmod nf7_driver
	dmesg | tail -n 20
