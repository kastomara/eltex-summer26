obj-m += netlink.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc user.c -o user
mode:
	@sudo insmod netlink.ko
	@sudo dmesg | tail -10
1:
	@./user
	@sudo dmesg | tail -10
10:
	@./user "Max number of feats is equal to 10"
	@sudo dmesg | tail -10
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@sudo rmmod netlink
