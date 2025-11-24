#pragma once

#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <pthread.h>



void StartMqueue(       int*    cid, 
                        mqd_t*  qid_in, 
                        mqd_t*  qid_out, 
                        int*    created_in, 
                        int*    created_out);


pthread_t StartReceiver(mqd_t   qid, 
                        int     cid);

void StartSender(       mqd_t   qid,    
                        int     cid);

void* ReceiverThread(   void*   arg);
