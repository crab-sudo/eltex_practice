#include "backend.h"
#include "gui.h"
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc == 4){
        const unsigned int count = atoi(argv[3]);

        uint32_t IPlist[count];
        IPgenerator(count, IPlist);

        uint32_t IPgateway = inet_addr(argv[1]);
        uint32_t IPmask = inet_addr(argv[2]);

        uint32_t* TrueIPList = (uint32_t*)malloc(count * sizeof(uint32_t));

        uint in_subnet_count = IPAnalizator(IPgateway, IPmask, IPlist, count, TrueIPList);
        IPPrint(IPlist, TrueIPList, count, in_subnet_count);

        free(TrueIPList);

        printf("\nКоличество адресов в подсети: %u\n", in_subnet_count);

    }
    else {
        printf("\033[41m%s\033[0m\n", "Недостаточное количество аргументов (IP шлюза, маска подсети, кол-во проверяемых адресов)");
    }
    return 0;
}

// 172.16.0.1 255.255.0.0 1000000
// 10.0.0.1 255.0.0.0 10000