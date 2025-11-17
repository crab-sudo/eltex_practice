#include "backend.h"

int PromptTerminal(const char* prompt, char* argv[]) {
    static char buffer[MAX_LEN];
    int argc = 0;

    strncpy(buffer, prompt, MAX_LEN - 1);
    buffer[MAX_LEN - 1] = '\0';

    char *p = buffer;

    while (*p != '\0') {
        while (isspace((unsigned char)*p))  p++;

        if (*p == '\0')                     break;
        argv[argc++] = p;
        if (argc >= MAX_ARGS - 1)           break;

        while (*p != '\0' && !isspace((unsigned char)*p))       p++;
        if (*p != '\0')                                         {   *p = '\0'; p++;    }
    }

    argv[argc] = NULL;
    return argc;

}