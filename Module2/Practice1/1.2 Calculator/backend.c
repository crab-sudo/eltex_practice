#include "backend.h"

STATUS sum( double* result, 
            double  a, 
            double  b)              {*result = a + b;    return CODE_0;}




STATUS diff(double* result, 
            double  a, 
            double  b)              {*result = a - b;    return CODE_0;}





STATUS multyplex(   double* result, 
                    double  a, 
                    double  b)      {*result = a * b;    return CODE_0;}





STATUS divide(      double* result, 
                    double  a, 
                    double  b){
                        
    if (b == 0.0) {
        return DIV_BY_ZERO;
    }
    *result = a / b;
    return CODE_0;
}



char* StatusToString(STATUS status){
    switch(status){
        case CODE_0:        return "CODE_0";        break;
        case DIV_BY_ZERO:   return "DIV_BY_ZERO";   break;
    }
}



STATUS (*Sellect(char operation))  (double*, double, double){
    switch(operation){
        case '+':   return sum;             break;
        case '-':   return diff;            break;
        case '*':   return multyplex;       break;
        case '/':   return divide;          break;
    }
}