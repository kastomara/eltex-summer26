obj-m += keylights.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
mod:	
	$(shell sudo insmod keylights.ko)
	@sudo dmesg | tail -10
1:
	$(shell echo "1" | sudo tee sys/kernel/systest/test)
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(shell sudo rmmod keylights)
