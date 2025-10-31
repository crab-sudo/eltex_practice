#include "backend.h"
#include "interface.h"

#include <string.h>
#include <stdlib.h>

int main(){
    STATUS (*function) (double*, double, double);

    PARS_STATUS pars_status = CODE_STATUS_0;
    STATUS      status      = CODE_0;
    char*       str_status  = "";

    double      result = 0, a = 0, b = 0;
    char        operation;
    char        prompt[15];

    while(1){
        printf("\n\nEnter command (or 'exit' to quit): ");
        if (!fgets(prompt, sizeof(prompt), stdin))      break;

        prompt[strcspn(prompt, "\n")] = '\0';

        if (strcmp(prompt, "exit") == 0) {
            printf("Exiting program...\n");
            break;
        }

        pars_status = PromptTerminal(prompt, &a, &b, &operation);

        if (pars_status != CODE_STATUS_0){
            ParsErrorPrint(pars_status);
            continue;
        }

        function    = Sellect(operation);
        status      = function(&result, a, b);
        str_status  = StatusToString(status);

        system("clear");

        if (strcmp(str_status, "CODE_0") != 0){
            FuncErrorPrint(str_status);
            continue;
        }
        ResultPrint(prompt, result);
    }
}

// tomato