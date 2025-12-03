#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>

int 			sock;
volatile sig_atomic_t 	running = 1;





struct 	sockaddr_in client1, client2;
int 	has_client1 = 0;
int 	has_client2 = 0;





void handle_sigint(int sig) {
    running = 0;
    printf("\nServer stopping...\n");
}





int main() {
    signal(SIGINT, handle_sigint);

    struct sockaddr_in 	server, client;
    char 		buffer[1024];
    socklen_t 		len = sizeof(client);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&server, sizeof(server));

    printf("Server started. Waiting for 2 clients...\n");

    while (running) {
        int bytes = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
                             (struct sockaddr*)&client, &len);

        if (bytes <= 0) continue;
        buffer[bytes] = '\0';

        
        if (!has_client1) {
            client1 = client;
            has_client1 = 1;
            printf("Client #1 connected.\n");
            continue;
        }

        if (!has_client2 &&
            (client.sin_port != client1.sin_port ||
             client.sin_addr.s_addr != client1.sin_addr.s_addr)) 
        {
            client2 = client;
            has_client2 = 1;
            printf("Client #2 connected.\n");
            continue;
        }

        if (!has_client1 || !has_client2) {
            printf("Waiting for second client...\n");
            continue;
        }

        if (client.sin_port == client1.sin_port &&
            client.sin_addr.s_addr == client1.sin_addr.s_addr) {

            printf("Client #1: %s\n", buffer);
            sendto(sock, buffer, bytes, 0,
                   (struct sockaddr*)&client2, len);
        } 
        else {
            printf("Client #2: %s\n", buffer);
            sendto(sock, buffer, bytes, 0,
                   (struct sockaddr*)&client1, len);
        }


        if (strcmp(buffer, "exit") == 0) {
            sendto(sock, "exit", 4, 0, (struct sockaddr*)&client1, len);
            sendto(sock, "exit", 4, 0, (struct sockaddr*)&client2, len);
            break;
        }
    }

    close(sock);
    printf("Server terminated.\n");
    return 0;
}
