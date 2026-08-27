Собираем и загружаем модуль. Можно сделать через:
```
make mode
```
<br> Попробуем что-то записать. Можно через make write или с использованием комманды:
```
echo "Ваш текст" | sudo tee /dev/chardev
```
<br>Читаем данные с носителя. Реализовано через make read.
```
sudo cat /dev/chardev
```
<br> Вывод сообщения всегда начиначется с приветствия, далее выводится user_data.
<br>Получаем:
<br>![Image](https://drive.google.com/uc?export=view&id=1Kgah_2PQfAiN9j37BJjpZ2fdex7VrWBC)
<br>Отключаем модуль. Встроено в make clean.
```bash
sudo rmmod procs
```