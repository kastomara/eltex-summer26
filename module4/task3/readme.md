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
<br>Открываем консоли компьютеров, настраиваем ip адреса:
<br>PC1 : ip 192.168.20.1/24
<br>PC2 : ip 192.168.33.2/24
<br>PC3 : ip 192.168.20.3/24
<br>PC4 : ip 192.168.33.4/24
<br>PC5 : ip 192.168.20.5/24
<br>PC6 : ip 192.168.33.6/24

<br>Пробуем пинговать компьютеры в одиннаковых VLAN:
```
PC1> ping 192.168.20.3

84 bytes from 192.168.20.3 icmp_seq=1 ttl=64 time=34.910 ms
84 bytes from 192.168.20.3 icmp_seq=2 ttl=64 time=31.183 ms
84 bytes from 192.168.20.3 icmp_seq=3 ttl=64 time=12.651 ms
84 bytes from 192.168.20.3 icmp_seq=4 ttl=64 time=11.226 ms
84 bytes from 192.168.20.3 icmp_seq=5 ttl=64 time=19.319 ms

PC5> ping 192.168.20.1

84 bytes from 192.168.20.1 icmp_seq=1 ttl=64 time=17.979 ms
84 bytes from 192.168.20.1 icmp_seq=2 ttl=64 time=25.864 ms
84 bytes from 192.168.20.1 icmp_seq=3 ttl=64 time=14.405 ms
84 bytes from 192.168.20.1 icmp_seq=4 ttl=64 time=11.238 ms
84 bytes from 192.168.20.1 icmp_seq=5 ttl=64 time=16.272 ms

PC4> ping 192.168.33.6

84 bytes from 192.168.33.6 icmp_seq=1 ttl=64 time=20.907 ms
84 bytes from 192.168.33.6 icmp_seq=2 ttl=64 time=17.389 ms
84 bytes from 192.168.33.6 icmp_seq=3 ttl=64 time=11.871 ms
84 bytes from 192.168.33.6 icmp_seq=4 ttl=64 time=17.887 ms
84 bytes from 192.168.33.6 icmp_seq=5 ttl=64 time=25.144 ms

PC2> ping 192.168.33.6/24

84 bytes from 192.168.33.6 icmp_seq=1 ttl=64 time=26.449 ms
84 bytes from 192.168.33.6 icmp_seq=2 ttl=64 time=16.059 ms
84 bytes from 192.168.33.6 icmp_seq=3 ttl=64 time=8.784 ms
84 bytes from 192.168.33.6 icmp_seq=4 ttl=64 time=28.400 ms
84 bytes from 192.168.33.6 icmp_seq=5 ttl=64 time=14.961 ms
```
<br>Теперь попробуем пингонуть компьютер из другого VLAN
```
PC4> ping 192.168.20.1

No gateway found

PC2> ping 192.168.20.5

No gateway found
```
<br>Закономерно, маршрут отсутствует, данные не могут добраться до адресата на этапе прохождения коммутатора.

<br>3) Перехватить в WireShark пакеты с тегами и без тегов (nb!), результаты задокументировать
<br>Делаем захват линка между любыми 2-мя коммутаторами, делаем пинг для соответсвующего VLAN. Получаем пары пакетов request и reply протокола ICMP между адресами компьютеров (строки destination и source address). Идентификатор (ID) равен 20, значит обмениваются пакетами компьютеры из VLAN20
```
802.1Q Virtual LAN, PRI: 0, DEI: 0, ID: 20
    000. .... .... .... = Priority: Best Effort (default) (0)
    ...0 .... .... .... = DEI: Ineligible
    .... 0000 0001 0100 = ID: 20
    Type: IPv4 (0x0800)
Internet Protocol Version 4, Src: 192.168.20.1, Dst: 192.168.20.3
    0100 .... = Version: 4
    .... 0101 = Header Length: 20 bytes (5)
    Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)
    Total Length: 84
    Identification: 0x25fb (9723)
    000. .... = Flags: 0x0
    ...0 0000 0000 0000 = Fragment Offset: 0
    Time to Live: 64
    Protocol: ICMP (1)
    Header Checksum: 0xab59 [validation disabled]
    [Header checksum status: Unverified]
    Source Address: 192.168.20.1
    Destination Address: 192.168.20.3
    [Stream index: 0]
```
<br>Получаем немного другие результаты при захвате линка, проводящем VLAN333. В отличие от предыдущего примера в графе Ethernet II нет id влана. Так выглядит нетегированный пакет.
```
Frame 28: Packet, 98 bytes on wire (784 bits), 98 bytes captured (784 bits) on interface -, id 0
Ethernet II, Src: Private_66:68:00 (00:50:79:66:68:00), Dst: Private_66:68:04 (00:50:79:66:68:04)
    Destination: Private_66:68:04 (00:50:79:66:68:04)
    Source: Private_66:68:00 (00:50:79:66:68:00)
    Type: IPv4 (0x0800)
    [Stream index: 5]
Internet Protocol Version 4, Src: 192.168.33.2, Dst: 192.168.33.6
    0100 .... = Version: 4
    .... 0101 = Header Length: 20 bytes (5)
    Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)
    Total Length: 84
    Identification: 0x2608 (9736)
    000. .... = Flags: 0x0
    ...0 0000 0000 0000 = Fragment Offset: 0
    Time to Live: 64
    Protocol: ICMP (1)
    Header Checksum: 0x9148 [validation disabled]
    [Header checksum status: Unverified]
    Source Address: 192.168.33.2
    Destination Address: 192.168.33.6
    [Stream index: 0]
Internet Control Message Protocol
```

<br> 4) Сохранить файлы конфигураций устройств в виде набора файлов с именами, соответствующими именам устройств
<br>Сохренены в папке conf.
<br>5*) Опциональное задание: Добавить в схему маршрутизатор, подключенный к коммутаторам Layer2Switch1 и Layer2Switch2, настроить через него маршрутизацию между VLAN
<br>Добавляем новые порты на 1 и 2 коммутатор. Подсоединяем к роуетеру.
<br>Первый коммутатор будет отправлять пакеты VLAN20. В консоли первого коммутатора прописываем:
```