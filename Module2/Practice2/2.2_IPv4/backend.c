#include "backend.h"

void IPgenerator(const unsigned int count, uint32_t IPlist[]) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    for (unsigned int i = 0; i < count; ++i) {
        uint8_t a = rand() % 256;
        uint8_t b = rand() % 256;
        uint8_t c = rand() % 256;
        uint8_t d = rand() % 256;

        uint32_t ip = ((uint32_t)a << 24) |
                      ((uint32_t)b << 16) |
                      ((uint32_t)c << 8)  |
                      ((uint32_t)d);

        IPlist[i] = ip;
    }
}



unsigned int IPAnalizator(  uint32_t            gateway, 
                            uint32_t            mask, 
                            uint32_t            IPlist[], 
                            const unsigned int  count,
                            uint32_t            TrueIPList[]) {
    uint32_t network = gateway & mask;
    unsigned int in_subnet_count = 0;

    for (unsigned int i = 0; i < count; ++i) {
        if ((IPlist[i] & mask) == network) {
            TrueIPList[in_subnet_count] = IPlist[i];
            in_subnet_count++;
        }
    }
    return in_subnet_count;
}