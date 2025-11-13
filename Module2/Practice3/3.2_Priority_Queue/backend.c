#include "backend.h"
#include <string.h>

void Init(Bucket* bucket) {
    for (int i = 0; i < 256; i++) {
        bucket->queue_list[i].front = NULL;
        bucket->queue_list[i].tail  = NULL;
    }
    bucket->current_max = -1;
    bucket->count       = 0;
}


void FreeAll(Bucket* bucket) {
    for (int i = 0; i < 256; i++) {
        Node* current = bucket->queue_list[i].front;
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp->data);
            free(temp);
        }
        bucket->queue_list[i].front = NULL;
        bucket->queue_list[i].tail  = NULL;
    }
    bucket->current_max = -1;
    bucket->count       = 0;
}


STATUS PackageClassificator (Bucket* bucket, const char* data, unsigned int priority) {
    if (priority > 255) return ERR_INVALID_PRIORITY;

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) return ERR_MEM_ALLOCATION_FAILED;

    newNode->data = strdup(data);
    if (!newNode->data) {
        free(newNode);
        return ERR_MEM_ALLOCATION_FAILED;
    }

    newNode->next = NULL;
    newNode->prev = NULL;

    Queue* q = &bucket->queue_list[priority];

    if (q->front == NULL) {
        q->front = q->tail = newNode;
    } else {
        newNode->prev = q->tail;
        q->tail->next = newNode;
        q->tail = newNode;
    }

    if ((int)priority > bucket->current_max) bucket->current_max = priority;

    bucket->count++;
    return CODE_0;
}


TrafficShare* DistributeTraffic(Queue* buckets, unsigned int traffic, const unsigned int newMin, int* outCount){
    int activePriorities[256];
    int activeCount             = 0;
    int totalPriority           = 0;

    for (int i = newMin; i < 256; i++) {
        if (buckets[i].front != NULL) {
            activePriorities[activeCount++] = i;
            totalPriority                   += i;
        }
    }

    if (activeCount == 0 || traffic == 0) {
        *outCount = 0;
        return NULL;
    }

    TrafficShare* result = (TrafficShare*)calloc(activeCount, sizeof(TrafficShare));
    if (!result) return NULL;

    int totalAssigned = 0;

    for (int i = 0; i < activeCount; i++) {
        int     pr      = activePriorities[i];
        double  weight  = (double)pr / (double)totalPriority;
        int     packets = (int)floor(weight * traffic);

        result[i].priority  = pr;
        result[i].packets   = packets;
        totalAssigned       += packets;
    }

    int remaining = traffic - totalAssigned;
    for (int i = activeCount - 1; i >= 0 && remaining > 0; i--) {
        result[i].packets++;
        remaining--;
    }

    *outCount = activeCount;
    return result;
}


STATUS WRR(Bucket* bucket, Queue* wrr_queue, unsigned int trafic, const unsigned int newMin) {
    if (!bucket || !wrr_queue || bucket->count == 0) return ERR_EMPTY_QUEUE;

    while (bucket->count > 0) {
        int activeCount;
        TrafficShare* share = DistributeTraffic(bucket->queue_list, trafic, newMin, &activeCount);

        if (!share || activeCount == 0) break; 

        for (int i = 0; i < activeCount - 1; i++) {
            for (int j = i + 1; j < activeCount; j++) {
                if (share[i].priority < share[j].priority) {
                    TrafficShare temp = share[i];
                    share[i] = share[j];
                    share[j] = temp;
                }
            }
        }

        for (int i = 0; i < activeCount; i++) {
            int pr = share[i].priority;
            int packets = share[i].packets;

            Node* node = bucket->queue_list[pr].front;
            while (packets-- > 0 && node) {
                Node* next = node->next;

                bucket->queue_list[pr].front = next;
                if (next) next->prev = NULL;
                else bucket->queue_list[pr].tail = NULL;
                bucket->count--;

                node->next              = NULL;
                node->prev              = wrr_queue->tail;
                if (wrr_queue->tail) wrr_queue->tail->next = node;
                else wrr_queue->front   = node;
                wrr_queue->tail         = node;

                node = next;
            }
        }

        free(share);
    }

    int new_max = -1;
    for (int p = 255; p >= 0; --p)
        if (bucket->queue_list[p].front) { new_max = p; break; }
    bucket->current_max = new_max;

    return CODE_0;
}
