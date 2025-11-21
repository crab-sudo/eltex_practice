#pragma once

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN     256
#define MAX_ARGS    64
#define MAX_CMDS    16

int PromptTerminal(const char* prompt, char* commands[]);

int ParseCommand(char* cmd, char* argv[]);
