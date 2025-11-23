#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define SERVER_MTYPE 10

typedef enum{
    CODE_0,
    ERR_CREATE_KEY,
    ERR_GET_QID,
    ERR_DENY_ACCESS
} STATUS;

struct msgbuf {
    long    recipient;
    int     sender;
    char    text[256];
};

void ErrPrint(          STATUS status);

STATUS ConnectQueue(    int* qid);

STATUS RegisterClient(  int     qid,    int*    cid);

void* ReceiverThread(   void*   arg);

void StartReceiver(     int     qid,    int     cid);

void StartSender(       int     qid,    int     cid);
