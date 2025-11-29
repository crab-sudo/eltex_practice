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

        FILE *f = fopen(filename, "r");
        if (!f) {
            perror("fopen r");
            sem_post(sem);
            break;
        }

        // Читаем все строки файла
        char lines[1000][1024];
        int line_count = 0;
        int target_line = -1;
        char selected_numbers[1024] = {0};

        while (fgets(lines[line_count], sizeof(lines[0]), f) && line_count < 1000) {
            // Убираем символ новой строки
            char *newline = strchr(lines[line_count], '\n');
            if (newline) *newline = '\0';
            
            // Ищем первую необработанную строку
            if (target_line == -1 && strncmp(lines[line_count], "OK", 2) != 0) {
                target_line = line_count;
                strncpy(selected_numbers, lines[line_count], sizeof(selected_numbers)-1);
            }
            line_count++;
        }
        fclose(f);

        int found = 0;
        if (target_line != -1) {
            // Помечаем строку как обработанную
            snprintf(lines[target_line], sizeof(lines[0]), "OK %s", selected_numbers);
            
            // Перезаписываем весь файл
            f = fopen(filename, "w");
            if (f) {
                for (int i = 0; i < line_count; i++) {
                    fprintf(f, "%s\n", lines[i]);
                }
                fclose(f);
                found = 1;
            }
        }

        sem_post(sem);

        if (!found) {
            sleep(1);
            continue;
        }

        // Обработка чисел из строки
        int min = INT_MAX, max = INT_MIN, value;
        
        // Разбираем числа из исходной строки (selected_numbers)
        char *token = strtok(selected_numbers, " \t");
        while (token) {
            if (sscanf(token, "%d", &value) == 1) {
                if (value < min) min = value;
                if (value > max) max = value;
            }
            token = strtok(NULL, " \t");
        }

        if (min != INT_MAX)
            printf("Processed: min=%d max=%d\n", min, max);

        sleep(1);
    }

    sem_close(sem);
    return 0;
}
