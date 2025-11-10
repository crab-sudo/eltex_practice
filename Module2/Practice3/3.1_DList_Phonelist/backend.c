#include "backend.h"





void Init(Person** head) {
    *head = NULL;
}






void FreeAll(Person** head) {
    Person* tmp = *head;
    while (tmp) {
        Person* next = tmp->next;
        free(tmp);
        tmp = next;
    }
    *head = NULL;
}






void StatusToString(STATUS status, char new_status[15]) {
    if (status == CODE_0)               strcpy(new_status, "CODE_0"             );
    if (status == ERR_FULL_ARRAY)       strcpy(new_status, "ERR_FULL_ARRAY"     );
    if (status == ERR_NO_FIRST_NAME)    strcpy(new_status, "ERR_NO_FIRST_NAME"  );
    if (status == ERR_NO_SECOND_NAME)   strcpy(new_status, "ERR_NO_SECOND_NAME" );
    if (status == ERR_INCORECT_ID)      strcpy(new_status, "ERR_INCORECT_ID"    );
}






unsigned int GeneratorID(Person* head) {
    unsigned int id;
    bool exists;
    srand((unsigned int)time(NULL));
    do {
        exists = false;
        id = (rand() % 9000) + 1000;
        Person* tmp = head;
        while (tmp) {
            if (tmp->id == id) {
                exists = true;
                break;
            }
            tmp = tmp->next;
        }
    } while (exists);
    return id;
}





STATUS Add(Person** head,   char* fname, 
                            char* sname, 
                            char* pname, 
                            char* job, 
                            char* vacancy, 
                            char* nomb, 
                            char* email, 
                            char* mess) {

    if (!strcmp(fname, "-")) return ERR_NO_FIRST_NAME;
    if (!strcmp(sname, "-")) return ERR_NO_SECOND_NAME;

    Person* new_person = (Person*)malloc(sizeof(Person));

    new_person->id = GeneratorID(*head);
    strcpy( new_person->fname,   fname  );
    strcpy( new_person->sname,   sname  );
    strcpy( new_person->pname,   pname  );
    strcpy( new_person->job,     job    );
    strcpy( new_person->vacancy, vacancy);
    strcpy( new_person->nomb,    nomb   );
    strcpy( new_person->email,   email  );
    strcpy( new_person->mess,    mess   );

    new_person->next = NULL;
    new_person->prev = NULL;

    if (!*head) {
        *head = new_person;
        return CODE_0;
    }

    Person* tmp = *head;
    Person* prev = NULL;
    while (tmp && tmp->id < new_person->id) {
        prev = tmp;
        tmp = tmp->next;
    }

    new_person->next = tmp;
    new_person->prev = prev;
    if (prev) prev->next = new_person;
    else *head = new_person;
    if (tmp) tmp->prev = new_person;

    return CODE_0;
}





STATUS Edit(Person* head, unsigned int id,  char* fname, 
                                            char* sname, 
                                            char* pname, 
                                            char* job, 
                                            char* vacancy, 
                                            char* nomb, 
                                            char* email, 
                                            char* mess) {

    Person* tmp = head;
    while (tmp && tmp->id != id) tmp = tmp->next;
    if (!tmp) return ERR_INCORECT_ID;

    if (strcmp( fname,      "-"))   strcpy( tmp->fname,     fname  );
    if (strcmp( sname,      "-"))   strcpy( tmp->sname,     sname  );
    if (strcmp( pname,      "-"))   strcpy( tmp->pname,     pname  );
    if (strcmp( job,        "-"))   strcpy( tmp->job,       job    );
    if (strcmp( vacancy,    "-"))   strcpy( tmp->vacancy,   vacancy);
    if (strcmp( nomb,       "-"))   strcpy( tmp->nomb,      nomb   );
    if (strcmp( email,      "-"))   strcpy( tmp->email,     email  );
    if (strcmp( mess,       "-"))   strcpy( tmp->mess,      mess   );

    return CODE_0;
}






STATUS Delete(Person** head, unsigned int id) {
    Person* tmp = *head;
    while (tmp && tmp->id != id) tmp = tmp->next;
    if (!tmp) return ERR_INCORECT_ID;

    if (tmp->prev) tmp->prev->next = tmp->next;
    else *head = tmp->next;
    if (tmp->next) tmp->next->prev = tmp->prev;

    free(tmp);
    return CODE_0;
}





STATUS CommandParser(Person** head, char* format, ...) {
    STATUS status = CODE_0;
    unsigned int id = 0;
    char fname  [20]    = "-";
    char sname  [20]    = "-";
    char pname  [20]    = "-";
    char job    [30]    = "-";
    char vacancy[20]    = "-";
    char nomb   [17]    = "-";
    char email  [30]    = "-";
    char mess   [30]    = "-";

    va_list args;
    va_start(args, format);

    for (char* c = format; *c; c++) {
        if (*c != '%') continue;
        switch (*++c) {
            case 'a':
                status = Add(   head,   fname, 
                                        sname, 
                                        pname, 
                                        job, 
                                        vacancy, 
                                        nomb, 
                                        email, 
                                        mess);                  break;

            case 'x':
                status = Edit(  *head,  id, 
                                        fname, 
                                        sname, 
                                        pname, 
                                        job, 
                                        vacancy, 
                                        nomb, 
                                        email, 
                                        mess);                  break;

            case 'd':
                status = Delete(head, id);                      break;


            case 'i':
                id = (unsigned int)atoi(va_arg(args, char*));   break;

            case 'f':
                strcpy(fname, va_arg(args, char*));             break;

            case 's':
                strcpy(sname, va_arg(args, char*));             break;

            case 'p':
                strcpy(pname, va_arg(args, char*));             break;

            case 'j':
                strcpy(job, va_arg(args, char*));               break;

            case 'v':
                strcpy(vacancy, va_arg(args, char*));           break;

            case 'n':
                strcpy(nomb, va_arg(args, char*));              break;

            case 'e':
                strcpy(email, va_arg(args, char*));             break;
                
            case 'm':
                strcpy(mess, va_arg(args, char*));              break;
        }
    }

    va_end(args);
    return status;
}






void PersonToString(Person* head, char str_persons[][200], unsigned int* count) {
    unsigned int i = 0;
    Person* tmp = head;
    while (tmp) {
        sprintf(str_persons[i++], "| %-4u | %-15s | %-15s | %-15s | %-20s | %-20s | %-15s | %-15s | %-15s |",
                                    tmp->id, 
                                    tmp->fname, 
                                    tmp->sname, 
                                    tmp->pname, 
                                    tmp->job, 
                                    tmp->vacancy, 
                                    tmp->nomb, 
                                    tmp->email, 
                                    tmp->mess);
        tmp = tmp->next;
    }
    *count = i;
}