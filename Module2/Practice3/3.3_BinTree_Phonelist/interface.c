#include "interface.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>






static void safe_copy(char *dest, size_t size, const char *src) {
    if (src == NULL) {
        if (size > 0) dest[0] = '\0';
        return;
    }
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}





void PromptTerminal(char* prompt, char new_prompt[30], char arguments[9][30]) {
    strcpy(new_prompt, "");
    for (size_t i = 0; i < 9; ++i) strcpy(arguments[i], "");
    size_t arg_i = 0;
    const char* c = prompt;
    char word[30];
    char command = '\0';
    char spec = '\0';

    while (*c) {
        if (*c == '%') {
            c++;
            if (*c == 'a' || *c == 'x' || *c == 'd') { command = *c; continue; }
            spec = *c;
            char tmp[8];
            sprintf(tmp, "%%%c ", spec);
            strcat(new_prompt, tmp);
            c++;
            while (isspace((unsigned char)*c)) c++;
            int i = 0;
            while (*c && !isspace((unsigned char)*c) && i < (int)(sizeof(word) - 1))
                word[i++] = *c++;
            word[i] = '\0';
            if (arg_i < 9) safe_copy(arguments[arg_i++], sizeof(arguments[arg_i - 1]), word);
        } else c++;
    }
    if (command) {
        char tmp[8];
        sprintf(tmp, "%%%c ", command);
        strcat(new_prompt, tmp);
    }
}






void PersonsPrint(char str_persons[][200], unsigned int count) {
    printf("| %-4s | %-15s | %-15s | %-15s | %-20s | %-20s | %-15s | %-15s | %-15s |\n",
           "ID", 
           "First name", 
           "Second name", 
           "Patronymic", 
           "Job", 
           "Post", 
           "Phone number", 
           "Email", 
           "Messenger");
    for (size_t i = 0; i < count; i++)  printf("%s\n", str_persons[i]);
}






void ErrPrint(char status[15]) {
    if (!strcmp(status, "ERR_FULL_ARRAY"))      printf("\033[41m%s\n\n\033[0m", "Person list is full");
    if (!strcmp(status, "ERR_NO_FIRST_NAME"))   printf("\033[41m%s\n\n\033[0m", "Not enter first name");
    if (!strcmp(status, "ERR_NO_SECOND_NAME"))  printf("\033[41m%s\n\n\033[0m", "Not enter second name");
    if (!strcmp(status, "ERR_INCORECT_ID"))     printf("\033[41m%s\n\n\033[0m", "Incorrect ID");
}
