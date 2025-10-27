#include "backend.h"
#define KPERSONS 6

Person persons[KPERSONS];

int main(){

    STATUS status_list[10];

    Init(persons, KPERSONS);

    // %a - Вызов STATUS Add(Person* persons[], 
    //          char* fname, 
    //          char* sname, 
    //          char* pname, 
    //          char* job, 
    //          char* vacancy,
    //          char* nomb_tmp, 
    //          char* email, 
    //          char* mess);

    // %x - Вызов STATUS Edit(Person* persons[], 
    //          int id, 
    //          char* fname, 
    //          char* sname, 
    //          char* pname, 
    //          char* job, 
    //          char* vacancy, 
    //          char* nomb_tmp, 
    //          char* email, 
    //          char* mess);

    // %d - Вызов STATUS Delete(Person* persons[], int id);

    // %i - ID
    // %f - Имя
    // %s - Фамилия
    // %p - Отчество
    // %j - Место работы
    // %v - Должность
    // %n - Номер телефона
    // %e - email
    // %m - Ссылка на соц.сеть

    CommandParser("",);

    return 0;
}

// tomato tomato