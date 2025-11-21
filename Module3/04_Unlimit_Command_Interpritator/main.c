#include "backend.h"

int main() {
    char*   commands    [MAX_CMDS];
    char*   argv        [MAX_ARGS];
    char    prompt      [MAX_LEN];

    while (1) {

        printf("\033[36m\nEnter command (or 'exit' to quit): \033[0m");

        if (!fgets(prompt, sizeof(prompt), stdin)) break;
        prompt[strcspn(prompt, "\n")] = '\0';

        if (strcmp(prompt, "exit") == 0) {
            printf("Exiting program...\n");
            break;
        }

        int cmdCount = PromptTerminal(prompt, commands);
        if (cmdCount == 0) continue;

        int pipefd[2];
        int prev_fd = -1;

        for (int i = 0; i < cmdCount; i++) {

            if (i < cmdCount - 1) 
                if (pipe(pipefd) == -1) {   perror("pipe");     continue;   }

            pid_t pid = fork();
            if (pid < 0) {  perror("fork");     continue;   }

            if (pid == 0) {
                if (prev_fd != -1) {
                    dup2(prev_fd, STDIN_FILENO);
                    close(prev_fd);
                }

                if (i < cmdCount - 1) {
                    close(pipefd[0]);
                    dup2 (pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                }

                ParseCommand(commands[i], argv);

                execvp(argv[0], argv);
                perror("execvp");
                _exit(1);
            }

            if (prev_fd != -1)      close(prev_fd);

            if (i < cmdCount - 1) {     close(pipefd[1]);   prev_fd = pipefd[0];    }

            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}


// ls -l | wc -l

// echo one two three two three three | tr o 0 | sort | uniq | wc -l