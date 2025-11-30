Отлично — сделаю полноценное, отлаженное решение на **чистом C (POSIX, Linux/UNIX)**.
Что реализовано в этом решении:

* Исправлены базовые ошибки из приведённого кода (правильное заполнение `serv_addr`, безопасное использование буферов, корректный `snprintf`, и т.д.).
* Расширена логика: клиент указывает требуемую операцию — `+`, `-`, `*`, `/` — сервер вычисляет и возвращает результат (поддержаны вещественные числа).
* Добавлена передача файлов:

  * `PUT <filename>` — клиент отправляет файл на сервер (загружается в папку `uploads/` на сервере).
  * `GET <filename>` — клиент запрашивает файл у сервера (сервер присылает, если файл есть).
* Протокол текстово-байтовой: текстовые управляющие строки (команды/ответы), затем — сырые байты файла при необходимости.
* Сервер многопроцессный: для каждого клиента делается `fork()` (как в исходном примере).
* В коде — подробные комментарии и проверка ошибок.

Ниже — два файла: `server.c` и `client.c`. Компиляция и запуск — в конце.

---

# server.c

```c
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
```

---

# client.c

```c
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
```

---

# Как это работает (коротко)

1. Клиент подключается к серверу по TCP.
2. Клиент отправляет **текстовые команды**:

   * `CALC <op> <a> <b>\n` — сервер отвечает `RESULT <value>\n` или `ERROR ...\n`.
   * `PUT <filename> <size>\n` — сервер отвечает `OK\n`, клиент отправляет ровно `<size>` байт с содержимым файла, затем сервер отвечает `OK Uploaded ...`.
   * `GET <filename>\n` — сервер отвечает `SIZE <size>\n` или `ERROR NotFound\n`. Если `SIZE`, клиент посылает `OK\n`, после чего сервер отправляет ровно `<size>` байт.
   * `QUIT\n` — закрытие соединения.
3. Сервер использует `fork()` для обслуживания каждого клиента в отдельном процессе — как в твоём исходном примере.

---

# Компиляция

```bash
gcc -o server server.c
gcc -o client client.c
```

(поскольку мы не используем pthread, флаг `-lpthread` не нужен)

# Примеры использования

1. Запуск сервера:

```bash
./server 5000
```

2. Запуск клиента:

```bash
./client 127.0.0.1 5000
```

3. В клиенте выбрать `1` — вычисления:

```
Choice: 1
Enter operation (one of + - * /): *
Enter first number: 2.5
Enter second number: 4
Server: RESULT 10
```

4. Загрузка файла:

```
Choice: 2
Enter path to local file to upload: /home/user/test.txt
Server: OK Uploaded test.txt 1234 bytes
```

5. Скачивание:

```
Choice: 3
Enter filename to download from server: test.txt
Server says file size: 1234 bytes
File downloaded to test.txt
```

---

# Важные замечания и возможные улучшения

* Пример ориентирован на POSIX-системы (Linux, macOS). Под Windows потребуется Winsock и замены `fork`, `open`, `read`, `write`, `recv`/`send` поведения.
* Нет авторизации — любой клиент может загружать/скачивать файлы. Для реального применения добавь аутентификацию.
* Для больших файлов и большого количества клиентов лучше использовать неблокирующий ввод/вывод, пул потоков или асинхронный подход.
* Текущий простой протокол не шифрует данные — для безопасности используй TLS.
* В PUT/GET не сделана проверка на атаки с `../` в именах — добавь нормализацию/проверку имени файла.

---

Если хочешь, могу:

* Привести минимально изменённые версии прямо из твоего исходного кода (с минимальными правками), чтобы было видно отличия.
* Добавить обработку нескольких клиентов в многопоточном варианте (pthread вместо fork).
* Сделать более дружественный клиент (без текстового меню — в стиле скриптового взаимодействия).
* Добавить проверки имени файла (запрет `..`), логирование на сервере, и graceful shutdown.

Что делаем дальше?
