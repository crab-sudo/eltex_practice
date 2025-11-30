#pragma once

#include <sys/types.h>

#define FTOK_FILE       "tmp_ftok"
#define FTOK_PROJ_ID    'S'
#define MAX_ITEMS       100
#define SHM_PERMS       0666


struct Shared {
    int count;
    int numbers[MAX_ITEMS];
    int min;
    int max;
};
