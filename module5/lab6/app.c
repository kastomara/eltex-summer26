#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

int main() {
    int fd = socket(PF_NETLINK, SOCK_RAW, 31);
    struct sockaddr_nl addr = { .nl_family = AF_NETLINK, .nl_groups = 1 };
    char buf[512];

    bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    printf("Ждем заблокированные пакеты...\n");

    while (1) {
        recv(fd, buf, sizeof(buf), 0);
        // Данные Netlink сообщения начинаются после заголовка nlmsghdr
        printf("%s\n", buf + sizeof(struct nlmsghdr));
    }
    return 0;
}