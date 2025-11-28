#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <limits.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};