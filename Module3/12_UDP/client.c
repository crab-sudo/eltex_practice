#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

int sock;
volatile int running = 1;

void* recv_thread(void* arg) {
    char buffer[1024];
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    while (running) {
        int bytes = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
                             (struct sockaddr*)&server, &len);

        if (bytes <= 0) continue;
        
        buffer[bytes] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            printf("\nServer requested termination.\n");
            running = 0;
            break;
        }

        printf("\nFrom server: %s\n", buffer);
        printf("You: ");
        fflush(stdout);
    }

    return NULL;
}

int main() {
    struct sockaddr_in server;
    pthread_t thread;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    pthread_create(&thread, NULL, recv_thread, NULL);

    char message[1024];

    while (running) {
        printf("You: ");
        fgets(message, sizeof(message), stdin);

        message[strcspn(message, "\n")] = 0;

        sendto(sock, message, strlen(message), 0,
               (struct sockaddr*)&server, sizeof(server));

        if (strcmp(message, "exit") == 0) {
            running = 0;
            break;
        }
    }

    pthread_join(thread, NULL);
    close(sock);

    printf("Client terminated.\n");
    return 0;
}
