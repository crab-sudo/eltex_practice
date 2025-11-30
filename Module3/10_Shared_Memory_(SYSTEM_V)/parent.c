#include "shared.h"

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <errno.h>


volatile sig_atomic_t stop_requested = 0;




static int SemopRetry(int semid, struct sembuf *ops, size_t nsops) {
    while (1) {
        if (semop(semid, ops, nsops) == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        return 0;
    }
}





void SigintHandler(int s) {
    (void)s;
    stop_requested = 1;
}





int main() {
    key_t   key;
    int     shmid, semid;
    struct  Shared *shm;
    int     processed   = 0;


    FILE *f = fopen(FTOK_FILE, "a");
    if (f) fclose(f);

    key = ftok(FTOK_FILE, FTOK_PROJ_ID);
    if (key == -1) {
        perror("ftok");
        return 1;
    }



    

    shmid = shmget(key, sizeof(struct Shared), IPC_CREAT | SHM_PERMS);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    shm = shmat(shmid, NULL, 0);
    if (shm == (void*)-1) {
        perror("shmat parent");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }





    semid = semget(key, 2, IPC_CREAT | SHM_PERMS | IPC_EXCL);
    if (semid == -1) {
        if (errno == EEXIST)
            semid = semget(key, 2, SHM_PERMS);
        else {
            perror("semget");
            shmdt(shm);
            shmctl(shmid, IPC_RMID, NULL);
            return 1;
        }
    } else {
        unsigned short  init_vals[2]    = {0, 0};

        union semun { 
            int             val; 
            unsigned short  *array; 
        } arg;
        arg.array   = init_vals;
        semctl(semid, 0, SETALL, arg);
    }




    struct sigaction sa;
    sa.sa_handler   = SigintHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags     = 0;
    sigaction(SIGINT, &sa, NULL);




    srand(time(NULL) ^ getpid());

    printf("Parent started. Run ./child in another terminal.\n");
    printf("Press Ctrl+C to stop.\n");

    while (!stop_requested) {
        int count = (rand() % MAX_ITEMS) + 1;

        shm->count = count;
        for (int i = 0; i < count; i++)
            shm->numbers[i] = (rand() % 2000) - 1000;


        struct sembuf op = {SEM_PARENT_TO_CHILD, 1, 0};
        SemopRetry(semid, &op, 1);


        struct sembuf op2 = {SEM_CHILD_TO_PARENT, -1, 0};
        SemopRetry(semid, &op2, 1);

        printf("Set #%d: min=%d max=%d (count=%d)\n",
               processed + 1, shm->min, shm->max, count);

        processed++;
    }


    shm->count = 0;

    struct sembuf op = {SEM_PARENT_TO_CHILD, 1, 0};
    SemopRetry(semid, &op, 1);

    printf("\nSIGINT received. Total sets processed: %d\n", processed);

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
