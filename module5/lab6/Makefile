obj-m += firewall.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc app.c -o app
mode:
	@sudo insmod firewall.ko
	@sudo dmesg | tail -10 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@sudo rmmod firewall
