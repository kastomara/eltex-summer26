## Тема: Настройка протокола DHCP

<br>1) Для заданной на схеме schema-lab4 сети, состоящей из управляемых коммутаторов, маршрутизаторов и персональных компьютеров выполнить планирование и документирование адресного пространства в подсетях LAN1, LAN2, LAN3 и назначить статические адреса маршрутизаторам и динамическое конфигурирование адресов для VPC

<br>Для маршрутизатора R1 настройка ip-адрессов:
```
configure terminal
interface FastEthernet0/0
 ip address 10.0.0.1 255.255.255.252
 no shutdown
 exit
interface Ethernet2/0
 ip address 192.168.10.1 255.255.255.0
 no shutdown
 exit
interface Ethernet2/1
 ip address 192.168.20.1 255.255.255.0
 no shutdown
 exit
```
<br>Для маршрутизатора R2:
```
configure terminal
interface FastEthernet0/0
 ip address 10.0.0.2 255.255.255.252
 no shutdown
 exit
```
<br>2) Настроить сервер DHCP на маршрутизаторе R2 для обслуживания адресных пулов адресного пространства подсетей LAN1 и LAN2

<br>В терминале R2:
```
ip dhcp excluded-address 192.168.10.1
ip dhcp excluded-address 192.168.20.1

ip dhcp pool POOL_LAN1
 network 192.168.10.0 255.255.255.0
 default-router 192.168.10.1
 exit

ip dhcp pool POOL_LAN2
 network 192.168.20.0 255.255.255.0
 default-router 192.168.20.1
 exit
``

<br>3) Настроить статическую (nb!) маршрутизацию между подсетями
<br>Наcтройка DHCP Relay на R1:
```
interface Ethernet2/0
 ip helper-address 10.0.0.2
 exit
interface Ethernet2/1
 ip helper-address 10.0.0.2
 exit
```
<br>Настройка статистической маршрутизации на R2:
```
ip route 192.168.10.0 255.255.255.0 10.0.0.1
ip route 192.168.20.0 255.255.255.0 10.0.0.1
```
<br>4) Проверить работоспособность протокола DHCP и маршрутизации, выполнив ping между всеми VPC
<br>В терминале каждого компьютера вводим: 
ip dhcp
<br>На выходе получаем свободные ip-адреса для каждого пк.

<br>Пингуем любой из полученных айпишников.
```
 ping 192.168.20.3

84 bytes from 192.168.20.3 icmp_seq=1 ttl=63 time=26.748 ms
84 bytes from 192.168.20.3 icmp_seq=2 ttl=63 time=16.210 ms
84 bytes from 192.168.20.3 icmp_seq=3 ttl=63 time=20.576 ms
84 bytes from 192.168.20.3 icmp_seq=4 ttl=63 time=23.239 ms
84 bytes from 192.168.20.3 icmp_seq=5 ttl=63 time=31.918 ms
``
<br>Все работает.

<br>5) Перехватить в wireshark диалог одного из VPC с сервером DHCP, разобрать с комментариями
При захвате линка и вводе ip dhcp получаем 4 пакета, формирующих DORA:
<br>1. DHCP Discover
```
Dynamic Host Configuration Protocol (Discover)
    Message type: Boot Request (1)
    Hardware type: Ethernet (0x01)
    Hardware address length: 6
    Hops: 0
    Transaction ID: 0x060b8756
    Seconds elapsed: 0
    Bootp flags: 0x0000 (Unicast)
    Client IP address: 0.0.0.0
    Your (client) IP address: 0.0.0.0
    Next server IP address: 0.0.0.0
    Relay agent IP address: 0.0.0.0
    Client MAC address: Private_66:68:02 (00:50:79:66:68:02)
    Client hardware address padding: 00000000000000000000
    Server host name not given
    Boot file name not given
    Magic cookie: DHCP
    Option: (53) DHCP Message Type (Discover)
    Option: (12) Host Name
    Option: (61) Client identifier
    Option: (255) End
    Padding: 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```
<br>Широковещательный запрос (destination 255.255.255.255) от компьютера без адресса (0.0.0.0).

<br>2. DHCP Offer
```
Dynamic Host Configuration Protocol (Offer)
    Message type: Boot Reply (2)
    Hardware type: Ethernet (0x01)
    Hardware address length: 6
    Hops: 0
    Transaction ID: 0x060b8756
    Seconds elapsed: 0
    Bootp flags: 0x0000 (Unicast)
    Client IP address: 0.0.0.0
    Your (client) IP address: 192.168.20.2
    Next server IP address: 0.0.0.0
    Relay agent IP address: 192.168.20.1
    Client MAC address: Private_66:68:02 (00:50:79:66:68:02)
    Client hardware address padding: 00000000000000000000
    Server host name not given
    Boot file name not given
    Magic cookie: DHCP
    Option: (53) DHCP Message Type (Offer)
    Option: (54) DHCP Server Identifier (10.0.0.2)
    Option: (51) IP Address Lease Time
    Option: (58) Renewal Time Value
    Option: (59) Rebinding Time Value
    Option: (1) Subnet Mask (255.255.255.0)
    Option: (3) Router
    Option: (255) End
    Padding: 0000000000000000000000000000000000000000
```
<br>R1 переслал Discover на R2. R2 выбрал свободный ip-адресс и отправил Offer обратно R1. Далее R1 отправляет Offer с адрессом, маской и шлюзом компьютеру.

<br>3. DHCP Request
```
Dynamic Host Configuration Protocol (Request)
    Message type: Boot Request (1)
    Hardware type: Ethernet (0x01)
    Hardware address length: 6
    Hops: 0
    Transaction ID: 0x060b8756
    Seconds elapsed: 0
    Bootp flags: 0x0000 (Unicast)
    Client IP address: 192.168.20.2
    Your (client) IP address: 0.0.0.0
    Next server IP address: 0.0.0.0
    Relay agent IP address: 0.0.0.0
    Client MAC address: Private_66:68:02 (00:50:79:66:68:02)
    Client hardware address padding: 00000000000000000000
    Server host name not given
    Boot file name not given
    Magic cookie: DHCP
    Option: (53) DHCP Message Type (Request)
    Option: (54) DHCP Server Identifier (10.0.0.2)
    Option: (50) Requested IP Address (192.168.20.2)
    Option: (61) Client identifier
    Option: (12) Host Name
    Option: (55) Parameter Request List
    Option: (255) End
    Padding: 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```
<br>Компьютер получает offer и делает запрос на использование данного ip-адресса. Запрос широковещательный, чтобы другие DHCP-серверы тоже его получили и не могли использовать ip-адресс.

<br>4. DHCP ACK
```
Dynamic Host Configuration Protocol (ACK)
    Message type: Boot Reply (2)
    Hardware type: Ethernet (0x01)
    Hardware address length: 6
    Hops: 0
    Transaction ID: 0x060b8756
    Seconds elapsed: 0
    Bootp flags: 0x0000 (Unicast)
    Client IP address: 192.168.20.2
    Your (client) IP address: 192.168.20.2
    Next server IP address: 0.0.0.0
    Relay agent IP address: 192.168.20.1
    Client MAC address: Private_66:68:02 (00:50:79:66:68:02)
    Client hardware address padding: 00000000000000000000
    Server host name not given
    Boot file name not given
    Magic cookie: DHCP
    Option: (53) DHCP Message Type (ACK)
    Option: (54) DHCP Server Identifier (10.0.0.2)
    Option: (51) IP Address Lease Time
    Option: (58) Renewal Time Value
    Option: (59) Rebinding Time Value
    Option: (1) Subnet Mask (255.255.255.0)
    Option: (3) Router
    Option: (255) End
    Padding: 0000000000000000000000000000000000000000
```
<br>Сервер подтверждает аренду ip-адресса пакетом Ackowledge.

<br>6) Сохранить файлы конфигураций устройств в виде набора файлов с именами, соответствующими именам устройств

<br>Конфиги сохранены в папке conf.