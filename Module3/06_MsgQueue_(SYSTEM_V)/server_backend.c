#include "server_backend.h"


void ErrPrint(STATUS status){
    if (status == ERR_NO_CID)       printf("\033[31m[ERROR] There is no recipient with this CID\n\033[0m");
    if (status == ERR_CREATE_KEY)   printf("\033[31m[ERROR] Key for creating queue not create\n\033[0m");
    if (status == ERR_CREATE_QID)   printf("\033[31m[ERROR] Msg queue not create\n\033[0m");
}




STATUS CreateQueue(int* qid){
    key_t key = ftok("tmp", 'Q');
    if (key  == -1)     return ERR_CREATE_KEY;

    *qid = msgget(key, IPC_CREAT | 0666);
    if (*qid == -1)     return ERR_CREATE_QID;

    return CODE_0;
}




int AddClient(  const int       qid,
                int             client_list[10],
                unsigned int*   client_count){

    int new_cid = ((*client_count + 1) + 1) * 10;

    client_list[*client_count] = new_cid;
    (*client_count)++;

    struct msgbuf out;
    out.recipient   = 1;
    out.sender      = 10;
    sprintf(out.text, "%d", new_cid);
    msgsnd(qid, &out, sizeof(out) - sizeof(long) - sizeof(int), 0);

    return new_cid;
}






STATUS ShutdownClient(int             client_list[10],   
                      unsigned int*   client_count,     
                      const int       cid){
    
    int i = 0;
    while (client_list[i] != cid)   i++;

    (*client_count)--;
    for (; i < *client_count; i++)   client_list[i] = client_list[i + 1];

    return CODE_0;
}




STATUS MSGCheck(const int            recipient, 
                const int            client_list[10],   
                const unsigned int   client_count){

    for (int i = 0; i < client_count; i++){
            if (client_list[i] == recipient)   return CODE_0;
        }

    return ERR_NO_CID;
}