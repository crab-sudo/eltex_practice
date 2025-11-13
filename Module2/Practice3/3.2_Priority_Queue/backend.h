#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef enum STATUS {
    CODE_0,
    ERR_MEM_ALLOCATION_FAILED,
    ERR_EMPTY_QUEUE,
    ERR_NO_TRAFFIC,
    ERR_INVALID_PRIORITY
}   STATUS;


typedef struct Node {
    char*           data;
    struct Node*    next;
    struct Node*    prev;
}   Node;

typedef struct Queue{
    Node*   front;
    Node*   tail;
}   Queue;

typedef struct Bucket {
    Queue           queue_list[256];
    unsigned int    current_max;
    unsigned int    count; 
}   Bucket;


typedef struct {
    int priority;
    int packets;
} TrafficShare;


void Init(                      Bucket*             bucket);

void FreeAll(                   Bucket*             bucket);

STATUS PackageClassificator(    Bucket*             bucket, 
                                const char*         data, 
                                unsigned int        priority);

TrafficShare* DistributeTraffic(Queue*              buckets, 
                                unsigned int        traffic, 
                                const unsigned int  newMin, 
                                int*                outCount);

STATUS WRR(                     Bucket*             bucket, 
                                Queue*              wrr_queue, 
                                unsigned int        trafic, 
                                const unsigned int  newMin);
