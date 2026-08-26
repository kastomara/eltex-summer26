obj-m += procs.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

mod:
	$(shell sudo insmod procs.ko)
1:
	$(shell sudo echo "Проверка провервка даблю даблю" > /proc/proc)
	$(shell cat /proc/proc)
2:
	$(shell sudo echo "Слишком большое сообщение вошло в чат и лишь какая-то его часть дойдет до конца увынск" > /proc/proc)
	$(shell cat /proc/proc)
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(shell sudo rmmod procs)
