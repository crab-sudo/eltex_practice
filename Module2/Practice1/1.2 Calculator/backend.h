#pragma once

#include <stdio.h>
#include <math.h>

typedef enum{
    CODE_0,
    DIV_BY_ZERO
}STATUS;

char* StatusToString(STATUS status);

STATUS sum( double* result, 
            double  a, 
            double  b);

STATUS diff(double* result, 
            double  a, 
            double  b);

STATUS multyplex(   double* result, 
                    double  a, 
                    double  b);

STATUS divide(      double* result, 
                    double  a, 
                    double  b);

STATUS (*Sellect(char operation))  (double*, double, double);