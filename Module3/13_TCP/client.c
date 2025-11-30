/* client.c
   TCP client for calculator and file transfer.
   Usage: ./client <host> <port>
   После запуска клиент предлагает интерактивное меню.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUF_SIZE 4096

ssize_t sendall(int sock, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;
    while (total < len) {
        ssize_t s = send(sock, p + total, len - total, 0);
        if (s <= 0) return -1;
        total += s;
    }
    return total;
}

ssize_t recvall(int sock, void *buf, size_t len) {
    size_t total = 0;
    char *p = buf;
    while (total < len) {
        ssize_t r = recv(sock, p + total, len - total, 0);
        if (r <= 0) return -1;
        total += r;
    }
    return total;
}

/* recvline: читает текстовую строку до '\n', записывает в buf */
ssize_t recvline(int sock, char *buf, size_t size) {
    size_t pos = 0;
    while (pos + 1 < size) {
        char c;
        ssize_t r = recv(sock, &c, 1, 0);
        if (r <= 0) return -1;
        if (c == '\r') continue;
        if (c == '\n') {
            buf[pos] = '\0';
            return pos;
        }
        buf[pos++] = c;
    }
    buf[pos] = '\0';
    return pos;
}

int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    printf("Connected to %s:%d\n", argv[1], portno);

    while (1) {
        printf("\nMenu:\n");
        printf("1) Calc (+ - * /)\n");
        printf("2) Upload file to server (PUT)\n");
        printf("3) Download file from server (GET)\n");
        printf("4) Quit\n");
        printf("Choice: ");
        int choice = 0;
        if (scanf("%d%*c", &choice) != 1) break;

        if (choice == 1) {
            char op;
            double a, b;
            printf("Enter operation (one of + - * /): ");
            if (scanf(" %c%*c", &op) != 1) break;
            printf("Enter first number: ");
            if (scanf("%lf%*c", &a) != 1) break;
            printf("Enter second number: ");
            if (scanf("%lf%*c", &b) != 1) break;

            char line[256];
            snprintf(line, sizeof(line), "CALC %c %.17g %.17g\n", op, a, b);
            if (sendall(sockfd, line, strlen(line)) < 0) {
                perror("send");
                break;
            }
            char resp[256];
            if (recvline(sockfd, resp, sizeof(resp)) <= 0) {
                printf("Server closed\n");
                break;
            }
            printf("Server: %s\n", resp);
        }
        else if (choice == 2) {
            char filename[512];
            printf("Enter path to local file to upload: ");
            if (fgets(filename, sizeof(filename), stdin) == NULL) break;
            filename[strcspn(filename, "\r\n")] = '\0';
            struct stat st;
            if (stat(filename, &st) != 0) {
                perror("stat");
                continue;
            }
            long long filesize = st.st_size;

            // Извлечь basename, чтобы на сервер сохранялось без путей
            char *base = strrchr(filename, '/');
            if (base) base++; else base = filename;

            char header[1024];
            snprintf(header, sizeof(header), "PUT %s %lld\n", base, filesize);
            if (sendall(sockfd, header, strlen(header)) < 0) {
                perror("send header");
                break;
            }
            // Ждём OK
            char resp[256];
            if (recvline(sockfd, resp, sizeof(resp)) <= 0) {
                printf("Server closed\n");
                break;
            }
            if (strncmp(resp, "OK", 2) != 0) {
                printf("Server response: %s\n", resp);
                continue;
            }
            // Отправляем файл
            int fd = open(filename, O_RDONLY);
            if (fd < 0) {
                perror("open");
                continue;
            }
            ssize_t r;
            char buf[BUF_SIZE];
            while ((r = read(fd, buf, sizeof(buf))) > 0) {
                if (sendall(sockfd, buf, r) != r) {
                    perror("send file");
                    break;
                }
            }
            close(fd);
            // Ждём финального подтверждения
            if (recvline(sockfd, resp, sizeof(resp)) <= 0) {
                printf("Server closed\n");
                break;
            }
            printf("Server: %s\n", resp);
        }
        else if (choice == 3) {
            char filename[512];
            printf("Enter filename to download from server: ");
            if (fgets(filename, sizeof(filename), stdin) == NULL) break;
            filename[strcspn(filename, "\r\n")] = '\0';

            char header[1024];
            snprintf(header, sizeof(header), "GET %s\n", filename);
            if (sendall(sockfd, header, strlen(header)) < 0) {
                perror("send");
                break;
            }
            // Ожидаем строку "SIZE <n>" или "ERROR ..."
            char resp[256];
            if (recvline(sockfd, resp, sizeof(resp)) <= 0) {
                printf("Server closed\n");
                break;
            }
            if (strncmp(resp, "SIZE ", 5) != 0) {
                printf("Server: %s\n", resp);
                continue;
            }
            long long filesize = atoll(resp + 5);
            printf("Server says file size: %lld bytes\n", filesize);

            // Подтверждаем готовность
            if (sendall(sockfd, "OK\n", 3) < 0) {
                perror("send");
                break;
            }
            // Открываем локальный файл для записи (перезаписать)
            char outpath[1024];
            snprintf(outpath, sizeof(outpath), "%s", filename);
            int fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open out");
                // Пропускаем чтение данных
                // можно читать и отбрасывать
                long long remaining = filesize;
                char tmpbuf[BUF_SIZE];
                while (remaining > 0) {
                    ssize_t toread = (remaining > BUF_SIZE) ? BUF_SIZE : (ssize_t)remaining;
                    ssize_t rr = recvall(sockfd, tmpbuf, toread);
                    if (rr <= 0) break;
                    remaining -= rr;
                }
                continue;
            }
            long long remaining = filesize;
            while (remaining > 0) {
                ssize_t toread = (remaining > BUF_SIZE) ? BUF_SIZE : (ssize_t)remaining;
                char buf[4096];
                ssize_t rr = recvall(sockfd, buf, toread);
                if (rr <= 0) {
                    perror("recv file");
                    break;
                }
                ssize_t w = write(fd, buf, rr);
                if (w != rr) {
                    perror("write file");
                    break;
                }
                remaining -= rr;
            }
            close(fd);
            if (remaining == 0) {
                printf("File downloaded to %s\n", outpath);
            } else {
                printf("Download incomplete\n");
            }
        }
        else if (choice == 4) {
            sendall(sockfd, "QUIT\n", 5);
            break;
        }
        else {
            printf("Unknown choice\n");
        }
    }

    close(sockfd);
    return 0;
}
