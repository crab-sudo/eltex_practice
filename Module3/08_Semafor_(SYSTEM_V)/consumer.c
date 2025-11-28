#include "consumer.h"





int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    char *filename = argv[1];

    key_t key = ftok(filename, 'S');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int semid = semget(key, 1, 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    struct sembuf P = {0, -1, 0};
    struct sembuf V = {0, +1, 0};



    while (1) {

        semop(semid, &P, 1);

        FILE *f = fopen(filename, "r+");
        if (!f) {
            perror("fopen");
            semop(semid, &V, 1);
            exit(1);
        }

        char line[256];
        long pos;
        int found = 0;

        while (1) {

            pos = ftell(f);
            if (!fgets(line, sizeof(line), f))
                break;

            if (strncmp(line, "OK ", 3) != 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            fclose(f);
            semop(semid, &V, 1);
            sleep(1);
            continue;
        }

        fseek(f, pos, SEEK_SET);
        fprintf(f, "OK ");
        fseek(f, pos + 3, SEEK_SET);
        fclose(f);

        semop(semid, &V, 1);


        char *p = line;

        if (strncmp(p, "OK ", 3) == 0)
            p += 3;

        int min = INT_MAX;
        int max = INT_MIN;

        int value;
        char *token = strtok(p, " ");
        while (token != NULL) {
            if (sscanf(token, "%d", &value) == 1) {
                if (value < min) min = value;
                if (value > max) max = value;
    }
    token = strtok(NULL, " ");
}


        printf("Consumer: min=%d max=%d\n", min, max);

        sleep(1);
    }

    return 0;
}
