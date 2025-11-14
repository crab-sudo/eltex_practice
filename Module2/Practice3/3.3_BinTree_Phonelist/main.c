#include "backend.h"
#include "interface.h"
#include <stdlib.h>

int main() {
    Person* head;
    char            str_persons [100][200];
    STATUS          status                  = CODE_0;
    char            new_status  [15];
    char            prompt      [200]       = "";
    char            new_prompt  [64];
    char            arguments   [9][30];
    unsigned int    count                   = 0;

    Init(&head);

    while (1) {
        PersonToString(head, str_persons, &count);
        PersonsPrint(str_persons, count);

        printf( "\n\n\033[33m%%a - Add()\n" );
        printf( "%%x - Edit()\n"            );
        printf( "%%d - Delete()\n\n"        );
            
        printf( "%%i - ID\n"                );
        printf( "%%f - First name\n"        );
        printf( "%%s - Second name\n"       );
        printf( "%%p - Patronymic\n"        );
        printf( "%%j - Job\n"               );
        printf( "%%v - Vacancy\n"           );
        printf( "%%n - Number\n"            );
        printf( "%%e - Email\n"             );
        printf( "%%m - Messenger\n\n"       );

        printf( "Example: %%a %%f Peter %%s Nepeypyvo %%j Magnit %%v Driver %%n 8-234-645-12-34\n\033[0m");

        printf( "\nEnter command (or 'exit' to quit): ");
        
        if (!fgets(prompt, sizeof(prompt), stdin)) break;
        prompt[strcspn(prompt, "\n")] = '\0';
        if (strcmp(prompt, "exit") == 0) {
            printf("Exiting program...\n");
            FreeAll(&head);
            break;
        }
        system("clear");
        PromptTerminal(prompt, new_prompt, arguments);
        status = CommandParser(&head, new_prompt,
                               arguments[0], arguments[1], arguments[2],
                               arguments[3], arguments[4], arguments[5],
                               arguments[6], arguments[7], arguments[8]);
        StatusToString(status, new_status);
        ErrPrint(new_status);
    }
    return 0;
}
