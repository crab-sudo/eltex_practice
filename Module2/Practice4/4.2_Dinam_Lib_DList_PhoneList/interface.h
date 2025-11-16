#pragma once
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void PromptTerminal(char* prompt, char new_prompt[30], char arguments[9][30]);

void PersonsPrint(char str_persons[][200], unsigned int count);

void ErrPrint(char status[15]);
