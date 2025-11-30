#pragma once

#include <sys/types.h>

#define FTOK_FILE       "/tmp/shm_sets_ftok"
#define FTOK_PROJ_ID    'S'
#define MAX_ITEMS       100
#define SHM_PERMS       0666


#define SEM_PARENT_TO_CHILD 0
#define SEM_CHILD_TO_PARENT 1


struct Shared {
    int count;
    int numbers[MAX_ITEMS];
    int min;
    int max;
};
