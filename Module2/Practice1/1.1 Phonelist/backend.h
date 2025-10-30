#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>



typedef enum {
    CODE_0,
    ERR_FULL_ARRAY,
    ERR_NO_FIRST_NAME,
    ERR_NO_SECOND_NAME,
    ERR_INCORECT_ID
} STATUS;



typedef struct {
    unsigned int    id;
    char            fname  [20];
    char            sname  [20];
    char            pname  [20];
    char            job    [30];
    char            vacancy[20];
    char            nomb   [17];
    char            email  [30];
    char            mess   [30];  
} Person;

void Init(Person* persons, size_t count);

STATUS Add(Person*        persons,
           unsigned int   MAXPERSONS,
           char*          fname, 
           char*          sname, 
           char*          pname, 
           char*          job, 
           char*          vacancy,
           char*          nomb_tmp, 
           char*          email, 
           char*          mess);


STATUS Edit(Person*       persons,
            unsigned int  MAXPERSONS, 
            unsigned int  id, 
            char*         fname, 
            char*         sname, 
            char*         pname, 
            char*         job, 
            char*         vacancy, 
            char*         nomb_tmp, 
            char*         email, 
            char*         mess);


STATUS Delete(Person* persons, unsigned int MAXPERSONS, unsigned int id);


STATUS CommandParser(Person*      persons,
                      unsigned int MAXPERSONS,
                      char*        format, 
                                   ...);


unsigned int GeneratorID(unsigned int* id_list, size_t size);

void StatusToString(STATUS      status, 
                    char        new_status_list[15]);

void PersonToString(Person*      persons, 
                    unsigned int MAXPERSONS, 
                    char         str_persons[][200]);