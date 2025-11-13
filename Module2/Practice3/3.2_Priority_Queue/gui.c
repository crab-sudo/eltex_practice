#include "gui.h"

GUI_STATUS PromptTerminal(  STATUS*             status, 
                            Bucket*             bucket, 
                            Queue*              wrr_queue, 
                            const unsigned int  trafic, 
                            char*               prompt) {

    char* token = strtok(prompt, " \t");
    unsigned int newMin = 0;
    if (!token) return GUI_ERR_UNKNOWN_COMMAND;

    if (!strcmp(token, "insert")) {
        char* p1 = strtok(NULL, " \t");
        char* p2 = strtok(NULL, " \t");
        if (!p1 || !p2) return GUI_ERR_UNKNOWN_COMMAND;

        unsigned int pr = strtol(p2, NULL, 10);
        if (pr > 255) return GUI_ERR_UNKNOWN_COMMAND;
        *status = PackageClassificator(bucket, p1, pr);
    }
    else if (!strcmp(token, "enqueue")) {
        char* mode = strtok(NULL, " \t");

        if (mode && !strcmp(mode, "first")){
            *status = WRR(bucket, wrr_queue, trafic, newMin);

            if (*status != CODE_0) return GUI_CODE_0;
            printf("First element in queue: %s\n\n", wrr_queue->front ? wrr_queue->front->data : "NULL");
        }
        else if (mode){
            int pr = atoi(mode);
            printf("Element with priority %d: %s\n\n", pr, bucket->queue_list[pr].front ? bucket->queue_list[pr].front->data : "NULL");
        
            *status = WRR(bucket, wrr_queue, trafic, newMin);
            if (*status != CODE_0) return GUI_CODE_0;
        }
        else
            return GUI_ERR_UNKNOWN_COMMAND;
    }
    else if (!strcmp(token, "enqueuenomin")) {
        newMin  = atoi(strtok(NULL, " \t"));
        *status = WRR(bucket, wrr_queue, trafic, newMin);

        if (*status != CODE_0) return GUI_CODE_0;
        printf("First element in queue with elements no min %d: %s\n\n", newMin, wrr_queue->front ? wrr_queue->front->data : "NULL");
    }
    else if (!strcmp(token, "clear"))       FreeAll(bucket);
    else return GUI_ERR_UNKNOWN_COMMAND;

    return GUI_CODE_0;
}


GUI_STATUS QueuePrint(Queue* wrr_queue) {
    Node* current = wrr_queue->front;
    while (current) {
        printf(" %s -> ", current->data);
        current = current->next;
    }
    return GUI_CODE_0;
}
