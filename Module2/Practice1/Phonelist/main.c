#include "backend.h"


int main(){
    unsigned int MAXPERSONS = 6;
    Person persons[MAXPERSONS];
    Init(persons, MAXPERSONS);
    STATUS status_list[10];

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

    CommandParser(status_list, persons, MAXPERSONS, "%f, %s, %p, %a",
                "Evgeniy",
                "Sharkov",
                "Evgenievich");

    printf("%u %s %s\n", persons[0].id, persons[0].fname, persons[0].sname);

    return 0;
}

// tomato tomato tomato