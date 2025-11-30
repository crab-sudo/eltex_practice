#include "shared.h"

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>


volatile sig_atomic_t stop_requested = 0;



void SigintHandler(int s) {
    (void)s;
    stop_requested = 1;
}




int main() {
    int     fd;
    struct  Shared *shm;
    int     processed   = 0;


    FILE *f = fopen(FTOK_FILE, "a");
    if (f) fclose(f);



    fd = shm_open("/shm_sets", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(fd, sizeof(struct Shared)) == -1) {
        perror("ftruncate");
        close(fd);
        shm_unlink("/shm_sets");
        return 1;
    }

    shm = mmap(NULL, sizeof(struct Shared),
               PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink("/shm_sets");
        return 1;
    }

    close(fd);





    sem_t *SEM_PARENT_TO_CHILD = sem_open("/SEM_PARENT_TO_CHILD", O_CREAT, 0666, 0);
    sem_t *SEM_CHILD_TO_PARENT = sem_open("/SEM_CHILD_TO_PARENT", O_CREAT, 0666, 0);

    if (SEM_PARENT_TO_CHILD == SEM_FAILED || SEM_CHILD_TO_PARENT == SEM_FAILED) {
        perror("sem_open");
        munmap(shm, sizeof(struct Shared));
        shm_unlink("/shm_sets");
        return 1;
    }



    struct sigaction sa;
    sa.sa_handler   = SigintHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags     = 0;
    sigaction(SIGINT, &sa, NULL);


    srand(time(NULL) ^ getpid());

    printf("Parent started (POSIX). Run ./child in another terminal.\n");
    printf("Press Ctrl+C to stop.\n");



    while (!stop_requested) {
        int count = (rand() % MAX_ITEMS) + 1;

        shm->count = count;
        for (int i = 0; i < count; i++)
            shm->numbers[i] = (rand() % 2000) - 1000;

        sem_post(SEM_PARENT_TO_CHILD);

        sem_wait(SEM_CHILD_TO_PARENT);

        printf("Set #%d: min=%d max=%d (count=%d)\n",
               processed + 1, shm->min, shm->max, count);

        processed++;
    }


    
    shm->count = 0;
    sem_post(SEM_PARENT_TO_CHILD);

    printf("\nSIGINT received. Total sets processed: %d\n", processed);



    munmap(shm, sizeof(struct Shared));
    shm_unlink("/shm_sets");

    sem_close(SEM_PARENT_TO_CHILD);
    sem_close(SEM_CHILD_TO_PARENT);
    sem_unlink("/SEM_PARENT_TO_CHILD");
    sem_unlink("/SEM_CHILD_TO_PARENT");

    return 0;
}
