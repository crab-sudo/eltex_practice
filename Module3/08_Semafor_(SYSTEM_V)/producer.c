#include "producer.h"



int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    char *filename = argv[1];

    FILE *check = fopen(filename, "a");
    if (!check) {
        perror("fopen");
        exit(1);
    }
    fclose(check);

    key_t key = ftok(filename, 'S');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }


    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf P = {0, -1, 0};
    struct sembuf V = {0, +1, 0};

    srand(time(NULL));

    while (1) {
        int count = rand() % 8 + 3;
        char line[256] = {0};

        for (int i = 0; i < count; i++) {
            char num[16];
            sprintf(num, "%d ", rand() % 100);
            strcat(line, num);
        }
        strcat(line, "\n");

        semop(semid, &P, 1);

        FILE *f = fopen(filename, "a");
        fputs(line, f);
        fclose(f);

        semop(semid, &V, 1);

        printf("Producer wrote: %s", line);
        sleep(1);
    }

    return 0;
}
