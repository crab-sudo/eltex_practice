#include "backend.h"


void Init(Person* persons, size_t count){
    for (int i = 0; i <= count; i++){
        persons[i].id = 0;
    }
}



void CommandParser(STATUS** status_list, Person* persons[], char* format, ...){
    
    STATUS status;

    unsigned int    id;
    char            fname[20]   = "-";
    char            sname[20]   = "-";
    char            pname[20]   = "-";
    char            job[30]     = "-";
    char            vacancy[20] = "-";
    char            nomb[17]    = "-";
    char            email[30]   = "-";
    char            mess[30]    = "-"; 

    va_list prompt;
    va_start(prompt, format);

    for (char *c = format; *c; c++){
        if (*c != '%')  continue;

            switch(*++c){
                case 'a':
                    printf("%s %s %s %s %s %s %s %s\n", 
                        fname, sname, pname, job, vacancy, nomb, email, mess);
                    //status = Add(&persons, fname, sname, pname, job, vacancy, nomb, email, mess);
                    break;
                
                case 'x':
                    printf("%s %s %s %s %s %s %s %s\n", 
                        fname, sname, pname, job, vacancy, nomb, email, mess);
                    //status = Edit(&persons, fname, sname, pname, job, vacancy, nomb, email, mess);
                    break;

                case 'd':
                    printf("%u\n", id);
                    //status = Delete(&persons, id);
                    break;

                case 'i':   id = va_arg(prompt, int);               break;

                case 'f':   strcpy(fname,   va_arg(prompt, char*)); break;

                case 's':   strcpy(sname,   va_arg(prompt, char*)); break;

                case 'p':   strcpy(pname,   va_arg(prompt, char*)); break;

                case 'j':   strcpy(job,     va_arg(prompt, char*)); break;

                case 'v':   strcpy(vacancy, va_arg(prompt, char*)); break;

                case 'n':   strcpy(nomb,    va_arg(prompt, char*)); break;

                case 'e':   strcpy(email,   va_arg(prompt, char*)); break;
                
                case 'm':   strcpy(mess,    va_arg(prompt, char*)); break;
            }

    }
}



STATUS Add(Person* persons[], char* fname, char* sname, char* pname, char* job, char* vacancy,
            char* nomb_tmp, char* email, char* mess){
    
    // size_t count = sizeof(persons) / sizeof(persons[0]);
    // int id_list[count];   
    // int i, i_this = 0;
    // while (persons[i]->id != 0){
    //     id_list[i] = persons[i]->id;
    //     ++i;
    //     if (i==count + 1)   return ERR_FULL_ARRAY;
    // }

    // strcpy(persons[i]->fname, fname);
    return CODE_0;
}



STATUS Edit(Person* persons[], int id, char* fname, char* sname, char* pname, char* job, char* vacancy,
            char* nomb_tmp, char* email, char* mess){

    int i = 0;
    while(persons[i]->id != id) ++i;

    if (strcmp(fname, "-"))     fname    = persons[i]->fname;
    if (strcmp(sname, "-"))     sname    = persons[i]->sname;
    if (strcmp(pname, "-"))     pname    = persons[i]->pname;
    if (strcmp(job, "-"))       job      = persons[i]->job;
    if (strcmp(vacancy, "-"))   vacancy  = persons[i]->vacancy;
    if (strcmp(nomb_tmp, "-"))  nomb_tmp = persons[i]->nomb;
    if (strcmp(email, "-"))     email    = persons[i]->email;
    if (strcmp(mess, "-"))      mess     = persons[i]->mess;   

    return CODE_0;
}