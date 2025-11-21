#include "backend.h"




int PromptTerminal(const char* prompt, char* commands[]) {
    static char buffer[MAX_LEN];
    strncpy(buffer, prompt, MAX_LEN - 1);
    buffer[MAX_LEN - 1] = '\0';

    int count = 0;
    char* p = buffer;

    while (*p != '\0') {
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;

        commands[count++] = p;
        if (count >= MAX_CMDS) break;

        while (*p != '\0' && *p != '|') p++;

        if (*p == '|') {
            *p = '\0';
            p++;
        }
    }

    commands[count] = NULL;
    return count;
}





int ParseCommand(char* cmd, char* argv[]) {
    int argc = 0;
    while (*cmd != '\0') {
        while (isspace((unsigned char)*cmd)) cmd++;
        if (*cmd == '\0') break;

        argv[argc++] = cmd;
        if (argc >= MAX_ARGS - 1) break;

        while (*cmd != '\0' && !isspace((unsigned char)*cmd)) cmd++;
        if (*cmd != '\0') { *cmd = '\0'; cmd++; }
    }

    argv[argc] = NULL;
    return argc;
}
