#pragma once

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN  64
#define MAX_ARGS 64


int PromptTerminal(const char* prompt, char* argv[]);