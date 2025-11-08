#include "backend.h"
#include "gui.h"
#include <sys/stat.h>
#include <stdio.h>

void main(int argc, char *argv[]) {
    STATUS status;
    char binary[10];
    char symbolyc[10];
    char nomb[4];

    if (argc == 2) {
        struct stat st;
        if (stat(argv[1], &st) == 0) {
            status = FileToPermission(argv[1], binary, nomb, symbolyc);
            if (status == CODE_0)
                printf("%s %s %s %s\n", symbolyc, nomb, binary, argv[1]);
        } 
        else if (isNombPermission(argv[1]) == CODE_0 || isSymbolicPermissions(argv[1]) == CODE_0) {

            status = PermissionToBinary(argv[1], binary);
            if (status == CODE_0)
                printf("%s %s\n", binary, argv[1]);
        }
        else {
            status = ERR_INCORRECT_FILENAME;
        }
    }
    else if (argc == 3) {
        struct stat st;
        if (stat(argv[1], &st) == 0) {
            status = EditPermissions(argv[1], argv[2], binary, nomb, symbolyc);
            if (status == CODE_0)
                printf("%s %s %s %s\n", symbolyc, nomb, binary, argv[1]);
        } 
        else if (isNombPermission(argv[1]) == CODE_0 || isSymbolicPermissions(argv[1]) == CODE_0) {
        }
        else {
            status = ERR_INCORRECT_FILENAME;
        }
    }

    Err_Print(StatusToString(status));
}
