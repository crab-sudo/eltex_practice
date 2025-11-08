#include "gui.h"

void Err_Print(const char* status){
        if (!strcmp(status, "ERR_INCORRECT_NOMB"))
            printf("\033[31m%s\n\033[0m", "Некорректная форма чистового представления прав доступа");            

        if (!strcmp(status, "ERR_INCORRECT_SYMBOLIC"))
            printf("\033[31m%s\n\033[0m", "Некорректная форма строкового представления прав доступа"); 

        if (!strcmp(status, "ERR_INCORRECT_ACCESS_PERMISSION"))
            printf("\033[31m%s\n\033[0m", "Некорректный формат прав доступа");    

        if (!strcmp(status, "ERR_INCORRECT_FILENAME"))
            printf("\033[31m%s\n\033[0m", "Файл не найден или некорректное имя файла"); 
}