obj-m += procs.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

mod:
	@sudo insmod procs.ko
1:
	@sudo echo "Проверка провервка даблю даблю" > /proc/proc
	@cat /proc/proc
2:
	@sudo echo "Слишком большое сообщение вошло в чат и лишь какая-то его часть дойдет до конца увынск" > /proc/proc
	@cat /proc/proc
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@sudo rmmod procs
