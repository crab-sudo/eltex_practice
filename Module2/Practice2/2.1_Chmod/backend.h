#pragma once

#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef enum {
    CODE_0,
    ERR_INCORRECT_NOMB,
    ERR_INCORRECT_SYMBOLIC,
    ERR_INCORRECT_ACCESS_PERMISSION,
    ERR_INCORRECT_FILENAME
}   STATUS;

char* StatusToString(STATUS status);

STATUS isNombPermission(                const char *s);
STATUS isSymbolicPermissions(           const char *s);

STATUS NombToBinary(                    const char* permission, char    binary[10]);
STATUS SymbolicToBinary(                const char* permission, char    binary[10]);

STATUS PermissionToBinary(              char*   permission,     char    binary[10]);

STATUS EditPermissionsNomb(             char*   modify,
                                        char    binary  [10],
                                        char    nomb    [4],
                                        char    symbolic[10]);

STATUS EditPermissionsSymbolicBinary(   char*   modify,
                                        char    binary  [10],
                                        char    nomb    [4],
                                        char    symbolic[10]);


STATUS FileToPermission(                char*   filename,       char    bynary  [10], 
                                                                char    nomb    [4], 
                                                                char    symbolic[10]);
                                                    
STATUS EditPermissions(                 char*   filename,       char*   modify, 
                                                                char    bynary  [10], 
                                                                char    nomb    [4], 
                                                                char    symbolic[10]);