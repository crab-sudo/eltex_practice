#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

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
        fprintf(stderr, "Usage: %s file.txt\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    char sem_name[256];
    make_sem_name_safe(filename, sem_name, sizeof(sem_name));

    sem_t *sem = sem_open(sem_name, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open consumer");
        return 1;
    }

    while (1) {
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            break;
        }

        FILE *f = fopen(filename, "r+");
        if (!f) {
            perror("fopen r+");
            sem_post(sem);
            break;
        }

        char line[1024];
        char selected_numbers[1024] = {0};
        long pos;
        int found = 0;

        while ((pos = ftell(f)), fgets(line, sizeof(line), f)) {

            if (strncmp(line, "OK", 2) == 0) continue;

            strncpy(selected_numbers, line, sizeof(selected_numbers)-1);

            fseek(f, pos, SEEK_SET);
            const char okprefix[9] = "OK ";
            fwrite(okprefix, 1, 8, f);
            fwrite(selected_numbers, 1, strlen(selected_numbers), f);
            fflush(f);

            found = 1;
            break;
        }

        fclose(f);
        sem_post(sem);

        if (!found) {
            usleep(200000);
            continue;
        }


        int min = INT_MAX, max = INT_MIN, value;
        char buf[1024];
        strncpy(buf, selected_numbers, sizeof(buf)-1);
        buf[sizeof(buf)-1] = '\0';

        char *p = buf;
        while (*p == ' ' || *p == '\t') p++;
        char *end = p + strlen(p) - 1;
        while (end >= p && strchr(" \t\r\n", *end)) *end-- = '\0';

        char *token = strtok(p, " \t");
        while (token) {
            if (sscanf(token, "%d", &value) == 1) {
                if (value < min) min = value;
                if (value > max) max = value;
            }
            token = strtok(NULL, " \t");
        }

        if (min == INT_MAX)
            printf("Consumer: nothing parsed from '%s'\n", selected_numbers);
        else
            printf("Processed: min=%d max=%d\n", min, max, selected_numbers);

        sleep(1);
    }

    sem_close(sem);
    return 0;
}
