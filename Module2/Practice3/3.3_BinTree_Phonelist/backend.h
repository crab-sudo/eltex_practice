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

typedef struct Person {
    unsigned int            id;
    char            fname   [20];
    char            sname   [20];
    char            pname   [20];
    char            job     [30];
    char            vacancy [20];
    char            nomb    [17];
    char            email   [30];
    char            mess    [30];
    struct Person*  left;
    struct Person*  right;
    int             height;
} Person;

void Init(Person** head);

void FreeAll(Person** head);

STATUS Add(Person** head,   char*   fname, 
                            char*   sname, 
                            char*   pname, 
                            char*   job, 
                            char*   vacancy, 
                            char*   nomb, 
                            char*   email, 
                            char*   mess);

STATUS Edit(Person* head,   unsigned int    id, 
                            char*   fname, 
                            char*   sname, 
                            char*   pname, 
                            char*   job, 
                            char*   vacancy, 
                            char*   nomb, 
                            char*   email, 
                            char*   mess);

STATUS Delete(Person** head, unsigned int id);

STATUS CommandParser(Person** head, char* format, ...);

unsigned int GeneratorID(Person* head);

void StatusToString(STATUS status, char new_status[15]);

void PersonToString(Person* head, char str_persons[][200], unsigned int* count);
