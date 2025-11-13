#include "backend.h"
#include "gui.h"
#include "test.h"
#include <string.h>

int main(int argc, char *argv[]) {
    GUI_STATUS      gui_status;
    STATUS          status;

    Bucket          bucket;
    Queue           wrr_queue;

    char            prompt[200] = "";
    unsigned int    trafic      = 30;

    Init(&bucket);
    wrr_queue.front = NULL;
    wrr_queue.tail  = NULL;
    int i = 0;

    const char**    test        = NULL;
    int             test_size   = 0;

    if (argc > 1) {
        if (strcmp(argv[1], "test1") == 0) {
            test = test1;
            test_size = test1_size;
        } 
        else if (strcmp(argv[1], "test2") == 0) {
            test = test2;
            test_size = test2_size;
        } 
        else if (strcmp(argv[1], "test3") == 0) {
            test = test3;
            test_size = test3_size;
        }
    }

    unsigned int use_test = (test != NULL);

    while (1) {
        gui_status = QueuePrint(&wrr_queue);

        printf("\n\n\033[33mCommands:\n\n");
        printf("insert          *char*    *priority (0-255)*\n");
        printf("enqueue         \"first\"   /   *priority (0-255)*\n");
        printf("enqueuenomin    *priority(0-255)*\n");
        printf("clear\033[0m\n\n");

        printf("\nEnter command (or 'exit' to quit): ");

        if (use_test) {
            if (i < test_size) {
                strcpy(prompt, test[i]);
            } else {
                strcpy(prompt, "exit");
            }
        } 
        else {
            if (!fgets(prompt, sizeof(prompt), stdin)) break;
            prompt[strcspn(prompt, "\n")] = '\0';
        }

        if (strcmp(prompt, "exit") == 0) {
            printf("Exiting program...\n");
            FreeAll(&bucket);
            break;
        }

        system("clear");
        gui_status = PromptTerminal(&status, &bucket, &wrr_queue, trafic, prompt);
        i++;
    }
    return 0;
}
