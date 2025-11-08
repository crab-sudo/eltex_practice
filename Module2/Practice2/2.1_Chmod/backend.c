#include "backend.h"


char* StatusToString(STATUS status){
    switch (status) {
        case CODE_0:                            return "CODE_0";                            break;
        case ERR_INCORRECT_NOMB:                return "ERR_INCORRECT_NOMB";                break;
        case ERR_INCORRECT_SYMBOLIC:            return "ERR_INCORRECT_SYMBOLIC";            break;
        case ERR_INCORRECT_ACCESS_PERMISSION:   return "ERR_INCORRECT_ACCESS_PERMISSION";    break;
        case ERR_INCORRECT_FILENAME:            return "ERR_INCORRECT_FILENAME";            break;
    }
}


// =========================== Проверка формата прав доступа ================================


STATUS isNombPermission(const char *s) {
    return strlen(s) == 3 && isdigit(s[0]) && isdigit(s[1]) && isdigit(s[2]) ? CODE_0 : ERR_INCORRECT_NOMB;
}





STATUS isSymbolicPermissions(const char *s) {
    if (!s || strlen(s) != 9)
        return ERR_INCORRECT_SYMBOLIC;

    for (int i = 0; i < 9; i++) {
        char c = s[i];
        switch (i % 3) {
            case 0:     if (c != 'r' && c != '-')   return ERR_INCORRECT_SYMBOLIC;   break;
            case 1:     if (c != 'w' && c != '-')   return ERR_INCORRECT_SYMBOLIC;   break;
            case 2:     if (c != 'x' && c != '-')   return ERR_INCORRECT_SYMBOLIC;   break;
        }
    }
    return CODE_0;
}





STATUS isSymbolicModifier(const char* modify) {
    if (modify == NULL || modify[0] == '\0')
        return ERR_INCORRECT_SYMBOLIC;

    char copy[64];
    strncpy(copy, modify, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';

    char* token = strtok(copy, ",");
    if (!token) return ERR_INCORRECT_SYMBOLIC;

    while (token) {
        int i = 0;
        int foundGroup = 0;

        while (token[i] == 'u' || token[i] == 'g' || token[i] == 'o' || token[i] == 'a') {
            foundGroup = 1;
            i++;
        }
        if (!foundGroup)
            foundGroup = 0; // допускаем отсутствие группы, как в "+x"

        if (token[i] != '+' && token[i] != '-' && token[i] != '=')
            return ERR_INCORRECT_SYMBOLIC;
        i++;

        int foundPerm = 0;
        while (token[i]) {
            if (token[i] != 'r' && token[i] != 'w' && token[i] != 'x')
                return ERR_INCORRECT_SYMBOLIC;
            foundPerm = 1;
            i++;
        }

        if (!foundPerm) return ERR_INCORRECT_SYMBOLIC;

        token = strtok(NULL, ",");
    }

    return CODE_0;
}




// =========================== Преобразование прав доступа ================================


STATUS NombToBinary(const char* permission, char binary[10]){
    binary[0] = '\0';

   for (int i = 0; permission[i] && i < 3; i++) {
        int val = permission[i] - '0';
        if (val < 0 || val > 7) {
            free(binary);
            return ERR_INCORRECT_ACCESS_PERMISSION;
        }

        sprintf(binary + i * 3, "%c%c%c",
                (val & 4) ? '1' : '0',
                (val & 2) ? '1' : '0',
                (val & 1) ? '1' : '0');
   }

    binary[9] = '\0';
    return CODE_0;
}






STATUS SymbolicToBinary(const char* permission, char binary[10]){
    binary[0] = '\0';
        for (int i = 0; i < 9; i += 3) {
            int val = 0;
            if (permission[i] == 'r') val += 4;
            if (permission[i + 1] == 'w') val += 2;
            if (permission[i + 2] == 'x') val += 1;
            sprintf(binary + (i / 3) * 3, "%c%c%c",
                    (val & 4) ? '1' : '0',
                    (val & 2) ? '1' : '0',
                    (val & 1) ? '1' : '0');
        }
        binary[9] = '\0';
        return CODE_0;
}





STATUS FileToPermission(char* filename, char    bynary  [10], 
                                        char    nomb    [4], 
                                        char    symbolic[10]){
    struct stat st;

    if (stat(filename, &st) != 0) {
        return ERR_INCORRECT_FILENAME;
    }

    int mode = st.st_mode & 0777;

    snprintf(nomb, 4, "%03o", mode);

    for (int i = 0; i < 3; i++) {
        int val = (mode >> (6 - 3*i)) & 7;

        bynary[i*3 + 0] = (val & 4) ? '1' : '0';
        bynary[i*3 + 1] = (val & 2) ? '1' : '0';
        bynary[i*3 + 2] = (val & 1) ? '1' : '0';

        symbolic[i*3 + 0] = (val & 4) ? 'r' : '-';
        symbolic[i*3 + 1] = (val & 2) ? 'w' : '-';
        symbolic[i*3 + 2] = (val & 1) ? 'x' : '-';
    }

    bynary[9]   = '\0';
    symbolic[9] = '\0';

    return CODE_0;
}



// ========================== Редактирование прав доступа ================================



STATUS EditPermissionsNomb(char* modify,
                           char binary[10],
                           char nomb[4],
                           char symbolic[10]) {

    strcpy(nomb, modify);

    binary[0] = '\0';
    symbolic[0] = '\0';

    for (int i = 0; i < 3; i++) {
        int val = modify[i] - '0';

        binary[i * 3 + 0]   = (val & 4) ? '1' : '0';
        binary[i * 3 + 1]   = (val & 2) ? '1' : '0';
        binary[i * 3 + 2]   = (val & 1) ? '1' : '0';

        symbolic[i * 3 + 0] = (val & 4) ? 'r' : '-';
        symbolic[i * 3 + 1] = (val & 2) ? 'w' : '-';
        symbolic[i * 3 + 2] = (val & 1) ? 'x' : '-';
    }

    binary[9] = '\0';
    symbolic[9] = '\0';

    return CODE_0;
}





STATUS EditPermissionsSymbolicBinary(char* modify,
                                     char binary[10],
                                     char nomb[4],
                                     char symbolic[10]) {

    int mask = 0;
    for (int i = 0; i < 9; i++) {
        if (binary[i] == '1') mask |= (1 << (8 - i));
    }

    char *token = strtok(modify, ",");
    while (token) {

        int applyMask = 0;
        int i = 0;
        for (; token[i]; i++) {
                    if (token[i] == 'u')    applyMask |= 0b111000000;
            else    if (token[i] == 'g')    applyMask |= 0b000111000;
            else    if (token[i] == 'o')    applyMask |= 0b000000111;
            else    if (token[i] == 'a')    applyMask = 0b111111111;
            else break;
        }
        if (!applyMask)                     applyMask = 0b111111111;

       
        int mode = 0;
                    if (token[i] == '+')    mode = 1;
        else        if (token[i] == '-')    mode = 2;
        else        if (token[i] == '=')    mode = 3;
        else return ERR_INCORRECT_SYMBOLIC;
        i++;

        int permMask = 0;
        for (; token[i]; i++) {
                    if (token[i] == 'r')    permMask |= 0b100100100;
            else    if (token[i] == 'w')    permMask |= 0b010010010;
            else    if (token[i] == 'x')    permMask |= 0b001001001;
            else return ERR_INCORRECT_SYMBOLIC;
        }

        permMask &= applyMask;

        switch(mode) {
            case 1: mask |= permMask;                       break;
            case 2: mask &= ~permMask;                      break;
            case 3: mask = (mask & ~applyMask) | permMask;  break;
        }

        token = strtok(NULL, ",");
    }

    for (int i = 0; i < 9; i++)
        binary[i] = (mask & (1 << (8 - i))) ? '1' : '0';
    binary[9] = '\0';

    for (int i = 0; i < 3; i++) {
        int val = (mask >> ((2 - i) * 3)) & 7;
        nomb[i] = val + '0';
    }
    nomb[3] = '\0';

    for (int i = 0; i < 3; i++) {
        int val = (mask >> ((2 - i) * 3)) & 7;
        symbolic[i*3 + 0] = (val & 4) ? 'r' : '-';
        symbolic[i*3 + 1] = (val & 2) ? 'w' : '-';
        symbolic[i*3 + 2] = (val & 1) ? 'x' : '-';
    }
    symbolic[9] = '\0';

    return CODE_0;
}







// =========================== Пользовательские функции ================================



STATUS PermissionToBinary(char* permission, char binary[10]){
    STATUS status = ERR_INCORRECT_ACCESS_PERMISSION;
    if (isNombPermission(permission) == CODE_0){
        status = NombToBinary(permission, binary);
        return status;
    }
    else if (isSymbolicPermissions(permission) == CODE_0){
        status = SymbolicToBinary(permission, binary);
        return status;
    }
    return status;
}




STATUS EditPermissions(char*filename,   char*   modify, char    bynary  [10], 
                                                        char    nomb    [4], 
                                                        char    symbolic[10]){
    STATUS status = FileToPermission(filename, bynary, nomb, symbolic);
    if (status != CODE_0)   return status;
    
    if (isNombPermission(modify) == CODE_0){
        status = EditPermissionsNomb(modify, bynary, nomb, symbolic);
        return status;
    }
    else if (isSymbolicModifier(modify) == CODE_0){
        status = EditPermissionsSymbolicBinary(modify, bynary, nomb, symbolic);
        return status;
    }
    else{
        return ERR_INCORRECT_ACCESS_PERMISSION;
    }
    return CODE_0;
}