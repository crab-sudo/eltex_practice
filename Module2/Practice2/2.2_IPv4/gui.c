#include "gui.h"

void IPPrint(   const uint32_t*     IPlist, 
                const uint32_t*     TrueIPList, 
                const unsigned int  count,
                const unsigned int  in_subnet_count) 
{
    printf("\n\033[33mВсе IP адреса:\033[0m\n");
    for (unsigned int i = 0; i < count; ++i) {
        uint32_t ip = ntohl(IPlist[i]);
        printf("%u.%u.%u.%u\n",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8)  & 0xFF,
            ip & 0xFF);
    }

    printf("\n\033[32mIP адреса в подсети:\033[0m\n");
    for (unsigned int i = 0; i < in_subnet_count; ++i) {
        uint32_t ip = ntohl(TrueIPList[i]);
        printf("%u.%u.%u.%u\n",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8)  & 0xFF,
            ip & 0xFF);
    }
}
