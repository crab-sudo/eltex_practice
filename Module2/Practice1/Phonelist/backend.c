#include "backend.h"


void Init(Person* persons, size_t count){
    for (int i = 0; i <= count; i++){
        persons[i].id = 0;
    }
}



void CommandParser(STATUS* status_list,
                   Person* persons, 
                   unsigned int MAXPERSONS, 
                   char* format, 
                   ...){
    
    STATUS status = CODE_0;

    unsigned int    id          = 0;
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
                    status = Add(persons, 
                                 MAXPERSONS, 
                                 fname, sname, pname, 
                                 job, 
                                 vacancy, 
                                 nomb, 
                                 email, 
                                 mess);

                    if (status == ERR_NO_FIRST_NAME) printf("%s\n", "ERR_NO_FIRST_NAME");
                    if (status == ERR_NO_SECOND_NAME) printf("%s\n", "ERR_NO_SECOND_NAME");
                    if (status == ERR_FULL_ARRAY) printf("%s\n", "ERR_FULL_ARRAY");
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






STATUS Add(Person* persons,
             unsigned int MAXPERSONS, 
             char* fname, 
             char* sname, 
             char* pname, 
             char* job, 
             char* vacancy,
             char* nomb_tmp, 
             char* email, 
             char* mess){
    

    if (!strcmp(fname, "-")) return ERR_NO_FIRST_NAME;
    if (!strcmp(sname, "-")) return ERR_NO_SECOND_NAME;

    unsigned int id_list[MAXPERSONS];   
    int i = 0;

    while (persons[i].id != 0){
        id_list[i] = persons[i].id;
        ++i;
        if (i == MAXPERSONS + 1)   return ERR_FULL_ARRAY;
    }

    persons[i].id = GeneratorID(id_list, MAXPERSONS);
    strcpy(persons[i].fname, fname);
    strcpy(persons[i].sname, sname);
    strcpy(persons[i].pname, pname);
    strcpy(persons[i].job, job);
    strcpy(persons[i].vacancy, vacancy);
    strcpy(persons[i].nomb, nomb_tmp);
    strcpy(persons[i].email, email);
    strcpy(persons[i].mess, mess);

    return CODE_0;
}





unsigned int GeneratorID(unsigned int* id_list, size_t size){
    unsigned int id;
    bool exists;
    
    srand((unsigned int)time(NULL));

    do {
        exists = false;
        id = (rand() % 9000) + 1000;

        for (size_t i = 0; i < size; i++) {
            if (id_list[i] == id) {
                exists = true;
                break;
            }
        }
    } while (exists);

    return id;
}





STATUS Edit(Person* persons, 
                int id, 
              char* fname, 
              char* sname, 
              char* pname, 
              char* job, 
              char* vacancy,
              char* nomb_tmp, 
              char* email, 
              char* mess){

    int i = 0;
    while(persons[i].id != id) ++i;

    if (strcmp(fname, "-"))     fname    = persons[i].fname;
    if (strcmp(sname, "-"))     sname    = persons[i].sname;
    if (strcmp(pname, "-"))     pname    = persons[i].pname;
    if (strcmp(job, "-"))       job      = persons[i].job;
    if (strcmp(vacancy, "-"))   vacancy  = persons[i].vacancy;
    if (strcmp(nomb_tmp, "-"))  nomb_tmp = persons[i].nomb;
    if (strcmp(email, "-"))     email    = persons[i].email;
    if (strcmp(mess, "-"))      mess     = persons[i].mess;   

    return CODE_0;
}