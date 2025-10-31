#pragma once

#include <stdio.h>

typedef enum{
    CODE_STATUS_0,
    ERR_NO_OPERATION,
    ERR_NO_TWO_NOMBERS
} PARS_STATUS;

PARS_STATUS PromptTerminal(char     prompt[15], 
                           double*  a, 
                           double*  b, 
                           char*    operation );


void ParsErrorPrint(PARS_STATUS pars_status);

void FuncErrorPrint();

void ResultPrint(char* prompt, double result);