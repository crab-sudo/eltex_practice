#include "backend.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>




typedef struct {
    mqd_t qid;
} recv_args;





void StartMqueue(   int*    cid, 
                    mqd_t*  qid_in, 
                    mqd_t*  qid_out, 
                    int*    created_in, 
                    int*    created_out) {

    const char  *name1          = "/queue1";
    const char  *name2          = "/queue2";
    struct      mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    *created_in     = 0;
    *created_out    = 0;

    mqd_t q1 = mq_open(name1, O_RDONLY);
    if (q1 != (mqd_t)-1) {
        *qid_in     = q1;
        *created_in = 0;

        mqd_t q2    = mq_open(name2, O_CREAT | O_RDWR, 0666, &attr);
        if (q2 == (mqd_t)-1) {
            perror("mq_open /queue2 (create) failed");
            mq_close(*qid_in);
            exit(EXIT_FAILURE);
        }
        *qid_out = q2;
        *created_out = 1;
        *cid = 2;
        return;
    } else {
        if (errno != ENOENT) {
            perror("mq_open /queue1 (read) failed");
            exit(EXIT_FAILURE);
        }

        mqd_t q1_out = mq_open(name1, O_CREAT | O_RDWR, 0666, &attr);
        if (q1_out == (mqd_t)-1) {
            perror("mq_open /queue1 (create) failed");
            exit(EXIT_FAILURE);
        }
        *qid_out = q1_out;
        *created_out = 1;
        *cid = 1;

 
        while (1) {
            mqd_t q2 = mq_open(name2, O_RDONLY);
            if (q2 != (mqd_t)-1) {
                *qid_in = q2;
                *created_in = 0;
                break;
            } else {
                if (errno == ENOENT) {

                    usleep(100 * 1000);
                    continue;
                } else {
                    perror("mq_open /queue2 (read) failed");

                    mq_close(*qid_out);
                    if (*created_out) mq_unlink(name1);
                    exit(EXIT_FAILURE);
                }
            }
        }
        return;
    }
}






void* ReceiverThread(void* arg) {
    recv_args*  p       = (recv_args*)arg;
    mqd_t       qid     = p->qid;
    free(p);

    size_t bufsize = 256;

    char *buf = malloc(bufsize + 1);
    if (!buf) {
        perror("malloc failed in receiver");
        return NULL;
    }

    while (1) {
        ssize_t n = mq_receive(qid, buf, bufsize, NULL);
        if (n >= 0) {
            if ((size_t)n > bufsize) n = bufsize;
            buf[n] = '\0';

            printf("\n%s\n> ", buf);
            fflush(stdout);

            if (strstr(buf, "Client shutdown") != NULL) {
                break;
            }
        } else {
            if (errno == EINTR) {
                continue;
            } else {
                perror("mq_receive failed");
                break;
            }
        }
    }

    free(buf);
    return NULL;
}






pthread_t StartReceiver(mqd_t qid, int cid) {
    pthread_t   th;
    recv_args*  args = malloc(sizeof(recv_args));
    if (!args) {
        perror("malloc failed in StartReceiver");
        exit(EXIT_FAILURE);
    }
    args->qid = qid;

    if (pthread_create(&th, NULL, ReceiverThread, args) != 0) {
        perror("pthread_create failed");
        free(args);
        exit(EXIT_FAILURE);
    }

    return th;
}







void StartSender(mqd_t qid, int cid) {
    while (1) {
        printf("> ");
        fflush(stdout);

        char line[256];
        if (!fgets(line, sizeof(line), stdin)) {

            snprintf(line, sizeof(line), "[FROM %d] Client shutdown", cid);
            if (mq_send(qid, line, strlen(line) + 1, 5) == -1) {
                perror("mq_send failed (EOF case)");
            }
            break;
        }

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "shutdown") == 0) {
            char msg[256];
            snprintf(msg, sizeof(msg), "[FROM %d] Client shutdown", cid);
            if (mq_send(qid, msg, strlen(msg) + 1, 5) == -1) {
                perror("mq_send failed (shutdown)");
            }
            break;
        }

        char msg[256];
        snprintf(msg, sizeof(msg), "[FROM %d] %s", cid, line);
        if (mq_send(qid, msg, strlen(msg) + 1, 5) == -1) {
            perror("mq_send failed");
        }
    }
}
