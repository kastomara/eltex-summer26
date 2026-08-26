Сборка и загрузка модуля:
![Image](https://drive.google.com/uc?export=view&id=1Z0zN092hQHMoxyqYQQP_oyUbH5_J2vZP)
<br>Проверяем его наличие в модулях ядра:
![Image](https://drive.google.com/uc?export=view&id=13r3PZF8d_nYL7WVYRdBTZ0aQl9lkNgMH)
![Image](https://drive.google.com/uc?export=view&id=1JMRMpeAfe0pa3Q8gekLq06hefaEb8NK4)
<br>Пробуем записать что-то менее установленного COUNT_SIZE
![Image](https://drive.google.com/uc?export=view&id=1oxDZxRegFrRSMUI1OvQd8M8JSgPwHFTI)
![Image](https://drive.google.com/uc?export=view&id=11PicrHeZOPknGJF3LdmYNnzvbjbfC7Uv)
<br>Пробуем записать что-то более установленного COUNT_SIZE. 
Получаем, что первые запись сдвигается на 50 байт вперед. При ровно 50 байт буффер перезаполняется и начинается запись следующей последовательности.
![Image](https://drive.google.com/uc?export=view&id=1xAnQIl8PzvQvBfXZ3vrtWEsGsJy_GTx1)
<br>Отключаем модуль.
