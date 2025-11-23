#include "client_backend.h"



typedef struct {
    int qid;
    int cid;
} recv_args;





void ErrPrint(STATUS status){
    if (status == ERR_CREATE_KEY)   printf("\033[31m[ERROR] Key for access to queue not create\n\033[0m");
    if (status == ERR_GET_QID)      printf("\033[31m[ERROR] Msg queue not create\n\033[0m");
    if (status == ERR_DENY_ACCESS)  printf("\033[31m[ERROR] Deny access\n\033[0m");
}




STATUS ConnectQueue(int* qid) {
    key_t key = ftok("tmp", 'Q');
    if (key  == -1)     return ERR_CREATE_KEY;

    *qid = msgget(key, 0666);
    if (*qid == -1)     return ERR_GET_QID;

    return CODE_0;
}





STATUS RegisterClient(int qid, int* cid) {
    struct msgbuf out;
    out.recipient = SERVER_MTYPE;
    out.sender    = 1;
    strcpy(out.text, "newclient");

    msgsnd(qid, &out, sizeof(out) - sizeof(long) - sizeof(int), 0);

    struct msgbuf msg;

    if (msgrcv(qid, &msg, sizeof(msg) - sizeof(long) - sizeof(int), 1, 0) == -1)
        return  ERR_DENY_ACCESS;

    *cid = atoi(msg.text);
    return CODE_0;
}






void* ReceiverThread(void* arg) {

    recv_args* p = (recv_args*)arg;
    int qid = p->qid;
    int cid = p->cid;

    struct msgbuf msg;

    while (1) {
        if (msgrcv(qid, &msg, sizeof(msg) - sizeof(long) - sizeof(int), cid, 0) != -1) {
            printf("\n[FROM %d] %s\n> ", msg.sender, msg.text);
            fflush(stdout);
        }
    }

    return NULL;
}





void StartReceiver(int qid, int cid) {
    pthread_t th;

    recv_args* args = malloc(sizeof(recv_args));
    args->qid = qid;
    args->cid = cid;

    pthread_create(&th, NULL, ReceiverThread, args);
    pthread_detach(th);
}





void StartSender(int qid, int cid) {

    while (1) {
        printf("> ");
        fflush(stdout);

        char buf[256];
        fgets(buf, sizeof(buf), stdin);
        buf[strcspn(buf, "\n")] = 0;

        int     recipient;
        char    text[256];

        int scanned = sscanf(buf, "%d %[^\n]", &recipient, text);

        if (scanned < 1) {
            if (strcmp(buf, "shutdown") == 0) {
                struct msgbuf msg;
                msg.recipient = SERVER_MTYPE;
                msg.sender    = cid;
                strcpy(msg.text, "shutdown");
                msgsnd(qid, &msg, sizeof(msg) - sizeof(long) - sizeof(int), 0);
                printf("Shutting down...\n");
                exit(0);
            }
            continue;
        }


        struct msgbuf msg;
        msg.recipient = SERVER_MTYPE;
        msg.sender    = cid;

        snprintf(msg.text, sizeof(msg) - sizeof(long) - sizeof(int), "%d %s", recipient, text);

        msgsnd(qid, &msg, sizeof(msg) - sizeof(long) - sizeof(int), 0);
    }
}

