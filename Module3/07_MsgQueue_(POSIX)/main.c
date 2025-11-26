#include "backend.h"

int main() {
    unsigned int    color       = 32;
    int             cid;
    mqd_t           qid_in      = (mqd_t)-1,    qid_out         = (mqd_t)-1;
    int             created_in  = 0,            created_out     = 0;


    mqd_t test1 = mq_open("/queue1", O_RDWR);
    mqd_t test2 = mq_open("/queue2", O_RDWR);

    if (test1 != (mqd_t)-1 && test2 != (mqd_t)-1) {
        printf("[SYSTEM] Both queues already exist. Removing them...\n");

        mq_close(test1);
        mq_close(test2);

        mq_unlink("/queue1");
        mq_unlink("/queue2");

        printf("[SYSTEM] Old queues removed.\n");
    } else {
        if (test1 != (mqd_t)-1) mq_close(test1);
        if (test2 != (mqd_t)-1) mq_close(test2);
    }


    StartMqueue(&cid, &qid_in, &qid_out, &created_in, &created_out);

    printf("\033[%dm[CLIENT %d] Created OUT queue: %d. Access to IN queue %d\n\033[0m",
           color, cid, (int)qid_out, (int)qid_in);

    pthread_t receiver_thread = StartReceiver(qid_in, cid);
    StartSender(qid_out, cid);

    pthread_join(receiver_thread, NULL);

    printf("[SYSTEM] shutdown\n");

    if (qid_out != (mqd_t)-1) mq_close(qid_out);
    if (qid_in  != (mqd_t)-1) mq_close(qid_in);

    if (created_out) {
        if (cid == 1) {
            if (mq_unlink("/queue1") == -1)
                perror("mq_unlink /queue1 failed");
            else
                printf("[SYSTEM] Unlinked /queue1\n");
        } else if (cid == 2) {
            if (mq_unlink("/queue2") == -1)
                perror("mq_unlink /queue2 failed");
            else
                printf("[SYSTEM] Unlinked /queue2\n");
        }
    }
    return 0;
}
