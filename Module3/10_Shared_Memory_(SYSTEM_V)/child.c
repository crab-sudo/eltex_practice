#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shared.h"


static int SemopRetry(int semid, struct sembuf *ops, size_t nsops) {
    while (1) {
        if (semop(semid, ops, nsops) == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        return 0;
    }
}






int main() {
    key_t key;
    int shmid, semid;
    struct Shared *shm;

    key = ftok(FTOK_FILE, FTOK_PROJ_ID);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    shmid = shmget(key, sizeof(struct Shared), SHM_PERMS);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    shm = shmat(shmid, NULL, 0);
    if (shm == (void*)-1) {
        perror("shmat");
        return 1;
    }

    semid = semget(key, 2, SHM_PERMS);
    if (semid == -1) {
        perror("semget");
        return 1;
    }

    printf("Child started. Waiting for data...\n");

    while (1) {

        struct sembuf op = {SEM_PARENT_TO_CHILD, -1, 0};
        SemopRetry(semid, &op, 1);

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

        struct sembuf op2 = {SEM_CHILD_TO_PARENT, 1, 0};
        SemopRetry(semid, &op2, 1);
    }

    shmdt(shm);
    return 0;
}
