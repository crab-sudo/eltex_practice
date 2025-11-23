#include "server_backend.h"

int main(){
    int             client_list[10];
    unsigned int    client_count    = 0;
    unsigned int    color           = 32;

    STATUS          status;

    int qid;

    status = CreateQueue(&qid);
    if (status != CODE_0){  ErrPrint(status);   exit(1);    }

    printf("\033[%dm[SERVER] Started. Queue ID = %d\n\033[0m", color, qid);

    while(1){
        struct msgbuf msg;

        if (msgrcv(qid, &msg, sizeof(msg) - sizeof(long) - sizeof(int), SERVER_MTYPE, 0) == -1){
            perror("msgrcv");
            continue;
        }

        if (strcmp(msg.text, "newclient") == 0){
            int new_cid = AddClient(qid, client_list, &client_count);
            printf("\033[%dm[SERVER] Add client with cid = %d\n\033[0m", color, new_cid);
            continue;
        }


        if (strcmp(msg.text, "shutdown") == 0){
            ShutdownClient(client_list, &client_count, msg.sender);
            printf("\033[%dm[SERVER] Shutdown client with cid = %d\n\033[0m", color, msg.sender);
            continue;
        }

        int target;
        char body[256] = {0};
        if (sscanf(msg.text, "%d %[^\n]", &target, body) < 1){
            printf("\033[31m[SERVER] Bad message format from %d: '%s'\n\033[0m", msg.sender, msg.text);
            continue;
        }

        printf("\033[%dm[SERVER] From %d to %d : %s\n\033[0m", color, msg.sender, target, body);

        status = MSGCheck(target, client_list, client_count);
        if (status != CODE_0){  ErrPrint(status);   continue;    }

        struct msgbuf out;
        out.recipient   = target;
        out.sender      = msg.sender;
        strcpy(out.text, body);

        if (msgsnd(qid, &out, sizeof(out) - sizeof(long) - sizeof(int), 0) == -1){
            perror("msgsnd (forward)");
        }
    }
    return 0;
}
