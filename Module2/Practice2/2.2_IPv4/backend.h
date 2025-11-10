#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

void IPgenerator(           const unsigned int  count,  uint32_t IPlist[]);

unsigned int IPAnalizator(  uint32_t            gateway, 
                            uint32_t            mask, 
                            uint32_t            IPlist[], 
                            const unsigned int  count,
                            uint32_t            TrueIPList[]);