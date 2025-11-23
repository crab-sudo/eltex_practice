#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_MTYPE 10

typedef enum{
    CODE_0,
    ERR_NO_CID,
    ERR_CREATE_KEY,
    ERR_CREATE_QID
} STATUS;

struct msgbuf{
    long    recipient;
    int     sender;
    char    text[256];
};

void ErrPrint(          STATUS status);

STATUS CreateQueue(     int*            qid);

int AddClient(          const int       qid,
                        int             client_list[10],   
                        unsigned int*   client_count);

STATUS ShutdownClient(  int             client_list[10],   
                        unsigned int*   client_count,     
                        const int       cid);

STATUS MSGCheck(        const int            recipient, 
                        const int            client_list[10],   
                        const unsigned int   client_count);