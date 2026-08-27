obj-m += chardev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
mode:
	@sudo insmod chardev.ko
	@sudo dmesg | tail -10 
read:
	@sudo cat /dev/chardev
write:
	echo "Write this one" | sudo tee /dev/chardev
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@sudo rmmod chardev
