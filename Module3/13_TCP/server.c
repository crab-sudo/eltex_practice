/* server.c
   TCP server: supports simple calculator operations and file upload/download.
   Usage: ./server <port>
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

#define BACKLOG 5
#define BUF_SIZE 4096
#define UPLOAD_DIR "uploads"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

/* sendall: отправляет exactly len байт через sock, или возвращает -1 при ошибке */
ssize_t sendall(int sock, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;
    while (total < len) {
        ssize_t sent = send(sock, p + total, len - total, 0);
        if (sent <= 0) return -1;
        total += sent;
    }
    return total;
}

/* recvall: читает ровно len байт (блокирует до получения или ошибки).
   возвращает количество прочитанных байт (равное len) или -1 при ошибке/закрытии. */
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

/* Читает одну текстовую строку (до '\n'), сохраняет в buf (size буфера),
   возвращает количество байт в строке (без '\n'), или -1 при ошибке/EOF.
   Символ '\n' удаляется из буфера (вместо него ставится '\0'). */
ssize_t recvline(int sock, char *buf, size_t size) {
    size_t pos = 0;
    while (pos + 1 < size) {
        char c;
        ssize_t r = recv(sock, &c, 1, 0);
        if (r <= 0) return -1;
        if (c == '\r') continue; // игнорируем CR
        if (c == '\n') {
            buf[pos] = '\0';
            return pos;
        }
        buf[pos++] = c;
    }
    buf[pos] = '\0';
    return pos;
}

void ensure_upload_dir_exists() {
    struct stat st;
    if (stat(UPLOAD_DIR, &st) != 0) {
        if (mkdir(UPLOAD_DIR, 0755) != 0) {
            perror("mkdir uploads");
            // не критично — продолжим, но попытки записать файл будут проваливаться
        }
    } else {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "uploads exists but is not a directory\n");
        }
    }
}

void handle_client(int newsockfd) {
    char line[1024];
    char outbuf[BUF_SIZE];
    ensure_upload_dir_exists();

    while (1) {
        ssize_t L = recvline(newsockfd, line, sizeof(line));
        if (L <= 0) break; // клиент закрыл соединение или ошибка

        // Простая парсер-команд: CALC <op> <num1> <num2>
        // или PUT <filename> <size>
        // или GET <filename>
        if (L == 0) continue;
        // printf("DEBUG: received line: '%s'\n", line);

        // Разбираем команду
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;

        if (strcmp(cmd, "CALC") == 0) {
            // CALC <op> <num1> <num2>
            char op;
            double a, b;
            int items = sscanf(line + strlen("CALC"), " %c %lf %lf", &op, &a, &b);
            if (items != 3) {
                snprintf(outbuf, sizeof(outbuf), "ERROR Invalid CALC syntax\n");
                sendall(newsockfd, outbuf, strlen(outbuf));
                continue;
            }
            double res;
            int ok = 1;
            if (op == '+') res = a + b;
            else if (op == '-') res = a - b;
            else if (op == '*') res = a * b;
            else if (op == '/') {
                if (b == 0.0) ok = 0;
                else res = a / b;
            } else ok = 0;

            if (!ok) {
                snprintf(outbuf, sizeof(outbuf), "ERROR Calculation error (bad op or division by zero)\n");
            } else {
                snprintf(outbuf, sizeof(outbuf), "RESULT %.10g\n", res);
            }
            sendall(newsockfd, outbuf, strlen(outbuf));
        }
        else if (strcmp(cmd, "PUT") == 0) {
            // PUT <filename> <size>
            char filename[512];
            long long filesize = 0;
            int items = sscanf(line + strlen("PUT"), " %511s %lld", filename, &filesize);
            if (items != 2 || filesize < 0) {
                snprintf(outbuf, sizeof(outbuf), "ERROR Invalid PUT syntax\n");
                sendall(newsockfd, outbuf, strlen(outbuf));
                continue;
            }

            // Открываем файл для записи в директорию uploads
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", UPLOAD_DIR, filename);

            int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                snprintf(outbuf, sizeof(outbuf), "ERROR Cannot create file\n");
                sendall(newsockfd, outbuf, strlen(outbuf));
                // пропускаем чтение тела, чтобы поток клиента не сойдёт с синхронизацией
                // лучше завершить соединение
                break;
            }

            // Подтверждаем готовность принимать
            snprintf(outbuf, sizeof(outbuf), "OK\n");
            sendall(newsockfd, outbuf, strlen(outbuf));

            // Читаем filesize байт и записываем в файл
            long long remaining = filesize;
            while (remaining > 0) {
                ssize_t toread = (remaining > BUF_SIZE) ? BUF_SIZE : (ssize_t)remaining;
                ssize_t r = recvall(newsockfd, outbuf, toread);
                if (r <= 0) {
                    close(fd);
                    unlink(path);
                    goto finish_client;
                }
                ssize_t w = write(fd, outbuf, r);
                if (w != r) {
                    close(fd);
                    unlink(path);
                    goto finish_client;
                }
                remaining -= r;
            }
            close(fd);
            snprintf(outbuf, sizeof(outbuf), "OK Uploaded %s %lld bytes\n", filename, filesize);
            sendall(newsockfd, outbuf, strlen(outbuf));
        }
        else if (strcmp(cmd, "GET") == 0) {
            // GET <filename>
            char filename[512];
            int items = sscanf(line + strlen("GET"), " %511s", filename);
            if (items != 1) {
                snprintf(outbuf, sizeof(outbuf), "ERROR Invalid GET syntax\n");
                sendall(newsockfd, outbuf, strlen(outbuf));
                continue;
            }
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", UPLOAD_DIR, filename);
            struct stat st;
            if (stat(path, &st) != 0) {
                snprintf(outbuf, sizeof(outbuf), "ERROR NotFound\n");
                sendall(newsockfd, outbuf, strlen(outbuf));
                continue;
            }
            long long filesize = st.st_size;
            // Отправляем заголовок SIZE
            snprintf(outbuf, sizeof(outbuf), "SIZE %lld\n", filesize);
            sendall(newsockfd, outbuf, strlen(outbuf));

            // Ожидаем подтверждение OK от клиента (чтобы синхронизироваться)
            ssize_t L2 = recvline(newsockfd, line, sizeof(line));
            if (L2 <= 0) break;
            if (strcmp(line, "OK") != 0) {
                // клиент не готов — пропускаем
                continue;
            }

            int fd = open(path, O_RDONLY);
            if (fd < 0) {
                snprintf(outbuf, sizeof(outbuf), "ERROR Cannot open file\n");
                sendall(newsockfd, outbuf, strlen(outbuf));
                continue;
            }
            // Отправляем файл поблочно
            ssize_t r;
            while ((r = read(fd, outbuf, sizeof(outbuf))) > 0) {
                if (sendall(newsockfd, outbuf, r) != r) {
                    break;
                }
            }
            close(fd);
            // После передачи можно отправить финальное сообщение (опционально)
        }
        else if (strcmp(cmd, "QUIT") == 0) {
            break;
        }
        else {
            snprintf(outbuf, sizeof(outbuf), "ERROR Unknown command\n");
            sendall(newsockfd, outbuf, strlen(outbuf));
        }
    }

finish_client:
    close(newsockfd);
    _exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    if (listen(sockfd, BACKLOG) < 0)
        error("ERROR on listen");

    printf("Server listening on port %d\n", portno);

    while (1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("accept");
            continue;
        }

        printf("Connection from %s:%d\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(newsockfd);
            continue;
        }
        if (pid == 0) {
            close(sockfd);
            handle_client(newsockfd);
            // never returns
        } else {
            close(newsockfd);
            // parent continues to accept()
        }
    }

    close(sockfd);
    return 0;
}
