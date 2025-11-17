#include "backend.h"

int main() {
    char*   argv    [MAX_ARGS];
    char    prompt  [MAX_LEN];
    int     argc                = 0;

    while (1) {
        
        printf( "\033[36m\nEnter command (or 'exit' to quit): \033[0m");
        
        if (!fgets(prompt, sizeof(prompt), stdin)) break;
        prompt[strcspn(prompt, "\n")] = '\0';
        if (strcmp(prompt, "exit") == 0) {
            printf("Exiting program...\n");
            break;
        }

        argc = PromptTerminal(prompt, argv);

        if (argc == 0)      continue;

        pid_t pid;

        switch (pid = fork()) {
            case -1:
                perror("Fork failed");
                continue;
            case 0:
                execvp(argv[0], argv);
                perror("execvp");
                _exit(1);
            default:
                waitpid(pid, NULL, 0);
        }

    }
    return 0;
}