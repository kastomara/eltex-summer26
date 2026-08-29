## Тема: Настройка виртуальной локальной сети (VLAN)

<br> 1) Для заданной на схеме schema-lab3 сети, состоящей из управляемых коммутаторов и персональных компьютеров настроить на коммутаторах логическую топологию используя протокол IEEE 802.1Q, для передачи пакетов VLAN333 между коммутаторами использовать Native VLAN

<br>Настройка для 1-го коммутатора:

<br>Настройка для 2-го коммутатора:
```
enable
configure terminal
vlan 20
name VLAN20
vlan 333
name VLAN333
exit

\* Мост для первого коммутатора*\

```
<br>Настройка для 3-го коммутатора:
```
enable
configure terminal
vlan 20
name VLAN20
vlan 333
name VLAN333
exit

/* Для PC1 */
interface GigabitEthernet1/0
switchport mode access
swithport access vlan 20
no shutdown

/* Для PC2 */
interface GigabitEthernet1/1
switchport mode access
swithport access vlan 333
no shutdown

/* Для Switch1 */
interface GigabitEthernet0/0
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 20
no shutdown
interface GigabitEthernet0/1
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 333
switchport trunk native vlan 333
no shutdown

/* Для Switch2 */
interface GigabitEthernet0/2
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 20
no shutdown
interface GigabitEthernet0/3
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 333
switchport trunk native vlan 333
no shutdown
```
<br>Настройка для 4-го коммутатора:
```
enable
configure terminal
vlan 20
name VLAN20
vlan 333
name VLAN333
exit

/* Для PC3 */
interface GigabitEthernet1/0
switchport mode access
swithport access vlan 20
no shutdown

/* Для PC4 */
interface GigabitEthernet1/1
switchport mode access
swithport access vlan 333
no shutdown

/* Для Switch1 */
interface GigabitEthernet0/0
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 20
no shutdown
interface GigabitEthernet0/1
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 333
switchport trunk native vlan 333
no shutdown

/* Для Switch2 */
interface GigabitEthernet0/2
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 20
no shutdown
interface GigabitEthernet0/3
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 333
switchport trunk native vlan 333
no shutdown
```

<br>Настройка для 5-го коммутатора:
```
enable
configure terminal
vlan 20
name VLAN20
vlan 333
name VLAN333
exit

/* Для PC5 */
interface GigabitEthernet1/0
switchport mode access
swithport access vlan 20
no shutdown

/* Для PC6 */
interface GigabitEthernet1/1
switchport mode access
swithport access vlan 333
no shutdown

/* Для Switch1 */
interface GigabitEthernet0/0
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 20
no shutdown
interface GigabitEthernet0/1
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 333
switchport trunk native vlan 333
no shutdown

/* Для Switch2 */
interface GigabitEthernet0/2
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 20
no shutdown
interface GigabitEthernet0/3
switchport trunk encapsulation dot1q
swithport mode trunk
switchport trunk allowed vlan 333
switchport trunk native vlan 333
no shutdown
```

<br> 2) Проверить доступность персональных компьютеров, находящихся в одинаковых VLAN и недоступность находящихся в различных, результаты задокументировать

<br> 3) Перехватить в WireShark пакеты с тегами и без тегов (nb!), результаты задокументировать

<br> 4) Сохранить файлы конфигураций устройств в виде набора файлов с именами, соответствующими именам устройств

5*) Опциональное задание: Добавить в схему маршрутизатор, подключенный к коммутаторам Layer2Switch1 и Layer2Switch2, настроить через него маршрутизацию между VLAN

Полезная информация: избыточные физические каналы можно поместить в отдельные VLAN и обойтись без STP