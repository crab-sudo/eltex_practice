#pragma once

#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

void IPPrint(   const uint32_t*     IPlist, 
                const uint32_t*     TrueIPList, 
                const unsigned int  count,
                const unsigned int  in_subnet_count);

