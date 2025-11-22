#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

volatile sig_atomic_t sigint_count      = 0;
volatile sig_atomic_t signal_received   = 0;
volatile sig_atomic_t last_signal       = 0;

void handler(int sig) {
    last_signal     = sig;
    signal_received = 1;

    if (sig == SIGINT)    sigint_count++;
}





int main() {
    FILE *fd = fopen("output.txt", "w");
    if (!fd) {
        perror("fopen");
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler               = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags                 = SA_RESTART;


    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT)");
        return 1;
    }
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction(SIGQUIT)");
        return 1;
    }

    int counter = 1;

    while (1) {
 
        if (signal_received) {
            if (last_signal == SIGINT)
                fprintf(fd, "Received and handled SIGINT (%d/3)\n", sigint_count);
            else if (last_signal == SIGQUIT)
                fprintf(fd, "Received and handled SIGQUIT\n");
            
            fflush(fd);
            signal_received = 0;
        }

        if (sigint_count >= 3) {
            fprintf(fd, "Received 3 SIGINTs, exiting.\n");
            fclose(fd);
            return 0;
        }

        fprintf(fd, "%d\n", counter++);
        fflush(fd);

        sleep(1);
    }

    fclose(fd);
    return 0;
}
