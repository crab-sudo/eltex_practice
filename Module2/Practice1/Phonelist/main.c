#include "backend.h"
#define KPERSONS 6

Person persons[KPERSONS];

int main(){

    STATUS status_list[10];

    Init(persons, KPERSONS);

    for (int i=0; i < KPERSONS; ++i)    printf ("%u\n", persons[i].id);

    return 0;
}

// tomato