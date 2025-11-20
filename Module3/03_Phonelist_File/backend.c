#include "backend.h"


void Init(Person* persons, size_t count){
    for (int i = 0; i < count; i++){
        persons[i].id = 0;
    }
}



void StatusToString(STATUS status, char new_status[30]){
        if (status == CODE_0)               strcpy(new_status, "CODE_0");
        if (status == ERR_FULL_ARRAY)       strcpy(new_status, "ERR_FULL_ARRAY");
        if (status == ERR_NO_FIRST_NAME)    strcpy(new_status, "ERR_NO_FIRST_NAME");
        if (status == ERR_NO_SECOND_NAME)   strcpy(new_status, "ERR_NO_SECOND_NAME");
        if (status == ERR_INCORECT_ID)      strcpy(new_status, "ERR_INCORECT_ID");
}



void PersonToString(Person* persons, unsigned int MAXPERSONS, char str_persons[][200]){
    
    for (size_t i = 0; i < MAXPERSONS; i++)     strcpy(str_persons[i], "");
    unsigned int i = 0, j = 0;
    while ( i < MAXPERSONS){
        if (persons[i].id != 0){
            sprintf(str_persons[j], "%-4d| %-15s| %-15s| %-15s| %-20s| %-20s| %-15s| %-15s| %-15s|", 
                    persons[i].id, 
                    persons[i].fname, 
                    persons[i].sname, 
                    persons[i].pname, 
                    persons[i].job, 
                    persons[i].vacancy, 
                    persons[i].nomb, 
                    persons[i].email, 
                    persons[i].mess);
                    ++j;
            }
            ++i;
    }
}




STATUS CommandParser(Person*      persons, 
                      unsigned int MAXPERSONS, 
                      char*        format, 
                                   ...){

    STATUS status = CODE_0;

    unsigned int    id          = 0;
    char            fname  [20] = "-";
    char            sname  [20] = "-";
    char            pname  [20] = "-";
    char            job    [30] = "-";
    char            vacancy[20] = "-";
    char            nomb   [17] = "-";
    char            email  [30] = "-";
    char            mess   [30] = "-"; 

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
                    break;
                
                case 'x':
                    status = Edit(persons, 
                                                 MAXPERSONS, 
                                                 id,
                                                 fname, sname, pname, 
                                                 job, 
                                                 vacancy, 
                                                 nomb, 
                                                 email, 
                                                 mess);
                    break;

                case 'd':
                    status = Delete(persons, MAXPERSONS, id);
                    break;

                case 'i':   id = (unsigned int)atoi(va_arg(prompt, char*));     break;

                case 'f':   strcpy(fname,   va_arg(prompt, char*));             break;

                case 's':   strcpy(sname,   va_arg(prompt, char*));             break;

                case 'p':   strcpy(pname,   va_arg(prompt, char*));             break;

                case 'j':   strcpy(job,     va_arg(prompt, char*));             break;

                case 'v':   strcpy(vacancy, va_arg(prompt, char*));             break;

                case 'n':   strcpy(nomb,    va_arg(prompt, char*));             break;

                case 'e':   strcpy(email,   va_arg(prompt, char*));             break;
                
                case 'm':   strcpy(mess,    va_arg(prompt, char*));             break;
            }

    }

    return status;
}






STATUS Add(Person*        persons,
           unsigned int   MAXPERSONS, 
           char*          fname, 
           char*          sname, 
           char*          pname, 
           char*          job, 
           char*          vacancy,
           char*          nomb, 
           char*          email, 
           char*          mess){
    

    if (!strcmp(fname, "-")) return ERR_NO_FIRST_NAME;
    if (!strcmp(sname, "-")) return ERR_NO_SECOND_NAME;

    unsigned int id_list[MAXPERSONS];   
    unsigned int i = 0;

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
    strcpy(persons[i].nomb, nomb);
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





STATUS Edit(Person*       persons,
            unsigned int  MAXPERSONS, 
            unsigned int  id, 
            char*         fname, 
            char*         sname, 
            char*         pname, 
            char*         job, 
            char*         vacancy, 
            char*         nomb, 
            char*         email, 
            char*         mess){

    int i = 0;
    while(persons[i].id != id){
        printf(":%d: - :%d:\n", persons[i].id, id);
        if (++i == MAXPERSONS)    return ERR_INCORECT_ID;
    }

    if (strcmp(fname,   "-"))   strcpy (persons[i].fname,   fname);
    if (strcmp(sname,   "-"))   strcpy (persons[i].sname,   sname);
    if (strcmp(pname,   "-"))   strcpy (persons[i].pname,   pname);
    if (strcmp(job,     "-"))   strcpy (persons[i].job,     job);
    if (strcmp(vacancy, "-"))   strcpy (persons[i].vacancy, vacancy);
    if (strcmp(nomb,    "-"))   strcpy (persons[i].nomb,    nomb);
    if (strcmp(email,   "-"))   strcpy (persons[i].email,   email);
    if (strcmp(mess,    "-"))   strcpy (persons[i].mess,    mess);   

    return CODE_0;
}




STATUS Delete(Person* persons, unsigned int MAXPERSONS, unsigned int id){

    unsigned int i = 0;
    while (persons[i].id != id)     if (++i == MAXPERSONS)   return ERR_INCORECT_ID;

    persons[i].id = 0;
    strcpy(persons[i].fname,   "");
    strcpy(persons[i].sname,   "");
    strcpy(persons[i].pname,   "");
    strcpy(persons[i].job,     "");
    strcpy(persons[i].vacancy, "");
    strcpy(persons[i].nomb,    "");
    strcpy(persons[i].email,   "");
    strcpy(persons[i].mess,    "");

    return CODE_0;
}





STATUS writePersons(const char *filename, const Person *array, int count)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)     return ERR_INCORECT_FD;

    const unsigned char *buf = (const unsigned char*)array;
    ssize_t to_write = (ssize_t)count * (ssize_t)sizeof(Person);
    ssize_t written = 0;

    while (written < to_write) {
        ssize_t w = write(fd, buf + written, (size_t)(to_write - written));
        if (w < 0) {
            if (errno == EINTR) continue;
            close(fd);
            return ERR_WRITE_FAILED;
        }
        written += w;
    }

    close(fd);
    return CODE_0;
}





STATUS readPersons(const char *filename, Person *array, int maxCount)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)     return ERR_INCORECT_FD;


    int count = 0;
    ssize_t r;
    while (count < maxCount) {
        r = read(fd, &array[count], sizeof(Person));
        if (r == sizeof(Person)) {
            ++count;
        } else if (r == 0) {
            break;
        } else if (r < 0) {
            if (errno == EINTR) continue;
            close(fd);
            return ERR_READ_FAILED;
        } else {
            break;
        }
    }

    close(fd);

    return CODE_0;
}
