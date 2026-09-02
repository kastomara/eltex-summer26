#include "udp_sniffer.h"

int main() {
    int choice;
    int chat_port = 0;
    int packet_count = 10;
    FilterType filter = FILTER_NONE;
    const char* filename = "capture_log.txt";

    printf("=== UDP Sniffer ===\n");
    printf("1. Фильтр: UDP Чат (по указанному порту)\n");
    printf("2. Фильтр: DNS (порт 53)\n");
    printf("Выберите фильтр (1-2): ");
    
    if (scanf("%d", &choice) != 1) {
        printf("Ошибка ввода.\n");
        return 1;
    }

    if (choice == 1) {
        filter = FILTER_UDP_CHAT;
        printf("Введите порт вашего UDP чата: ");
        scanf("%d", &chat_port);
    } else if (choice == 2) {
        filter = FILTER_DNS;
    } else {
        printf("Неверный выбор. Выход.\n");
        return 1;
    }

    printf("Введите количество пакетов для захвата (например, 5): ");
    scanf("%d", &packet_count);

    printf("\nЗапуск перехвата. Данные будут выводиться на экран и сохранены в файл '%s'.\n", filename);
    
    start_sniffer(filter, chat_port, packet_count, filename);

    return 0;
}