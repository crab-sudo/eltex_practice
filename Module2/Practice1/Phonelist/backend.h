#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef enum {
    CODE_0,
    ERR_FULL_ARRAY,
} STATUS;

#define ERR_FULL_ARRAY "ERR_FULL_ARRAY"

typedef struct {
    unsigned int    id;
    char            fname[20];
    char            sname[20];
    char            pname[20];
    char            job[30];
    char            vacancy[20];
    char            nomb[17];
    char            email[30];
    char            mess[30];  
} Person;

void Init(Person* persons, size_t count);

STATUS Add(Person* persons[], 
             char* fname, 
             char* sname, 
             char* pname, 
             char* job, 
             char* vacancy,
             char* nomb_tmp, 
             char* email, 
             char* mess);


STATUS Edit(Person* persons[], 
              int id, 
              char* fname, 
              char* sname, 
              char* pname, 
              char* job, 
              char* vacancy, 
              char* nomb_tmp, 
              char* email, 
              char* mess);


STATUS Delete(Person* persons[], int id);


void CommandParser(STATUS** status_list, 
                    Person* persons[], 
                      char* format, 
                            ...);