#include "backend.h"
#include "interface.h"

#include <termios.h>



int main(){
    const unsigned int  MAXPERSONS                     = 10;
    Person              persons    [MAXPERSONS];
    char                str_persons[MAXPERSONS][200];
    STATUS              status                         = CODE_0;
    char                new_status [30];

    char                prompt     [200]               = "";
    char                new_prompt [64];
    char                arguments  [9][30];

    Init(persons, MAXPERSONS);
    status = readPersons("phonebook.dat", persons, MAXPERSONS);

    while (1) {
        PersonToString(persons, MAXPERSONS, str_persons);

        PersonsPrint(str_persons, MAXPERSONS);

        // Пример промпта: %a %f Peter %s Nepeypyvo %j Magnit %v Driver %n 8-234-645-12-34
        // Пример с ошибкой: %a %s Krivozub %j Anywhere
        for(size_t i = 0; i < MAXPERSONS; i++)      printf("\n");
        printf("\033[33m%s\n%s\n%s\n\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n%s\033[0m", 
               "%a - Вызов Add()", 
               "%x - Вызов Edit()",
               "%d - Вызов Delete()",
               "%i - ID",
               "%f - Имя",
               "%s - Фамилия",
               "%p - Отчество",
               "%j - Место работы",
               "%v - Должность",
               "%n - Номер телефона",
               "%e - email",
               "%m - Ссылка на соц.сеть",
               "Пример промпта: %a %f Peter %s Nepeypyvo %j Magnit %v Driver %n 8-234-645-12-34");

        printf("\n\nEnter command (or 'exit' to quit): ");
        if (!fgets(prompt, sizeof(prompt), stdin))      break;
       
        prompt[strcspn(prompt, "\n")] = '\0';

        if (strcmp(prompt, "exit") == 0) {
            printf("Exiting program...\n");
            status = writePersons("phonebook.dat", persons, MAXPERSONS);
            StatusToString(status, new_status);
            ErrPrint(new_status);
            break;
        }

        system("clear");
        PromptTerminal(prompt, new_prompt, arguments);

        status = CommandParser(persons, MAXPERSONS, new_prompt,
                                    arguments[0],
                                    arguments[1],
                                    arguments[2],
                                    arguments[3],
                                    arguments[4],
                                    arguments[5],
                                    arguments[6],
                                    arguments[7],
                                    arguments[8]);

        StatusToString(status, new_status);
        ErrPrint(new_status);
    }

    return 0;
}