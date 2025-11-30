#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "shared.h"



int main() {
    int fd;
    struct Shared *shm;

    fd = shm_open("/shm_sets", O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    shm = mmap(NULL, sizeof(struct Shared),
               PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);


    sem_t *SEM_PARENT_TO_CHILD = sem_open("/SEM_PARENT_TO_CHILD", 0);
    sem_t *SEM_CHILD_TO_PARENT = sem_open("/SEM_CHILD_TO_PARENT", 0);

    if (SEM_PARENT_TO_CHILD == SEM_FAILED || SEM_CHILD_TO_PARENT == SEM_FAILED) {
        perror("sem_open");
        munmap(shm, sizeof(struct Shared));
        return 1;
    }


    printf("Child started (POSIX). Waiting for data...\n");

    while (1) {
        sem_wait(SEM_PARENT_TO_CHILD);

        if (shm->count == 0) {
            printf("Child: termination signal received.\n");
            break;
        }

        int count = shm->count;
        int mn = shm->numbers[0];
        int mx = shm->numbers[0];

        for (int i = 1; i < count; i++) {
            int v = shm->numbers[i];
            if (v < mn) mn = v;
            if (v > mx) mx = v;
        }

        shm->min = mn;
        shm->max = mx;

        sem_post(SEM_CHILD_TO_PARENT);
    }


    munmap(shm, sizeof(struct Shared));

    sem_close(SEM_PARENT_TO_CHILD);
    sem_close(SEM_CHILD_TO_PARENT);

    return 0;
}
