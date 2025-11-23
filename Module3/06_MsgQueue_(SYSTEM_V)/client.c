#include "client_backend.h"

int main() {
    STATUS status;
    int qid;
    int cid;

    status  = ConnectQueue(&qid);   
    if (status != CODE_0)   {   ErrPrint(status);  exit(1);  }
    
    status = RegisterClient(qid, &cid);
    if (status != CODE_0)   {   ErrPrint(status);  exit(1);  }

    printf("[CLIENT %d] Connected to %d\n", cid, qid);

    StartReceiver(qid, cid);
    StartSender(qid, cid);

    return 0;
}
