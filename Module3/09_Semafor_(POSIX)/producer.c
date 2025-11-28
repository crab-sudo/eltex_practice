#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>






void create_file_if_not_exists(const char *filename) {
    FILE *f = fopen(filename, "a");
    if (f) fclose(f);
}






void make_sem_name_safe(const char *filename, char *out, size_t outlen) {
    if (outlen == 0) return;
    size_t p = 0;
    out[p++] = '/';
    for (size_t i = 0; i < strlen(filename) && p + 1 < outlen; ++i) {
        char c = filename[i];
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9'))
            out[p++] = c;
        else
            out[p++] = '_';
    }
    out[p] = '\0';
}






int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./producer file.txt\n");
        return 1;
    }

    const char *filename = argv[1];
    create_file_if_not_exists(filename);

    char sem_name[256];
    make_sem_name_safe(filename, sem_name, sizeof(sem_name));

    sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    srand(time(NULL));

    while (1) {
        sem_wait(sem);

        FILE *f = fopen(filename, "a");
        if (!f) {
            perror("fopen");
            sem_post(sem);
            return 1;
        }

        int n = rand() % 6 + 3;
        char line[1024] = {0};
        for (int i = 0; i < n; i++) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d ", rand() % 100);
            strcat(line, buf);
        }
        strcat(line, "\n");

        fwrite(line, 1, strlen(line), f);
        fclose(f);

        printf("Produced: %s", line);

        sem_post(sem);
        sleep(1);
    }

    sem_close(sem);
    return 0;
}
