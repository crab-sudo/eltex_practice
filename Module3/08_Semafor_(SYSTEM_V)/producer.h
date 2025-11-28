#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
