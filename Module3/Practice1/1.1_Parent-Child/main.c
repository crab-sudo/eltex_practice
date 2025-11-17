#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>




int is_number(const char *s, double *result) {
    char *end;
    *result = strtod(s, &end);
    return end != s && *end == '\0';
}




void process_args(char *argv[], int start, int end, const char *prefix) {
    for (int i = start; i < end; i++) {
        double value;
        if (is_number(argv[i], &value))     printf("%s: %s -> %g\n",    prefix, argv[i], value * 2);
        else                                printf("%s: %s\n",          prefix, argv[i]);
    }
}




int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <args>\n", argv[0]);
        return 0;
    }

    int total = argc - 1;
    int mid = 1 + total / 2;

    pid_t pid;
    switch(pid = fork()){
        case -1:
            perror("fork failed");
            exit(EXIT_FAILURE);
        case 0:
            process_args(argv, mid, argc, "\033[33m<child>\033[0m");
            exit(EXIT_SUCCESS);
        default:
            process_args(argv, 1, mid, "\033[32m<parent>\033[0m");
            exit(EXIT_SUCCESS);
    }

    return 0;
}

// 1 h 2h h 3 4 5 6 hj j j j 5 6 3 k