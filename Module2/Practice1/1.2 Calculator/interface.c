#include "interface.h"
#include <stdlib.h>
#include <string.h>

PARS_STATUS PromptTerminal(char     prompt[15], 
                           double*  a, 
                           double*  b, 
                           char*    operation ){
    char    op          = '\0';
    char    tmp_prompt[15];
    strcpy(tmp_prompt, prompt);
    char*   ptr         = tmp_prompt;

    while (*ptr) {
        if (*ptr == '+' || *ptr == '-' || *ptr == '*' || *ptr == '/') {
            op = *ptr;
            *ptr = ' ';
            break;
        }
        ptr++;
    }

    if (op == '\0') {
        *operation = '\0';
        *a = *b = 0.0;
        return ERR_NO_OPERATION;
    }

    if (sscanf(tmp_prompt, "%lf %lf", a, b) != 2) {
        *operation = '\0';
        *a = *b = 0.0;
        return ERR_NO_TWO_NOMBERS;
    }

    *operation = op;
    return CODE_STATUS_0;
}



void ParsErrorPrint(PARS_STATUS pars_status){
    switch(pars_status){
        case ERR_NO_OPERATION:      printf("\033[41m%s\n\n\033[0m", "Не введён оператор");  break;
        case ERR_NO_TWO_NOMBERS:    printf("\033[41m%s", "Введено не два числа");           break;
    }
}



void FuncErrorPrint(char* str_status){
    if (strcmp(str_status, "DIV_BY_ZERO") == 0) {
        printf("\033[41m%s\n\n\033[0m", "Деление на ноль");
    }
}



void ResultPrint(char* prompt, double result){
    printf("%s = %f\n", prompt, result);
}