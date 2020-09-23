#include "Kernel/SyscallStubs.h"

int main() {
    const char data[] = "From the shell!\n";
    char results[] = " ";
    printf((char*)data);
    results[0] = '\0';
    int x = printf("> ");
    if (x == 100) {
        printf("Did it\n");
    }
    bool made = false;
    while (true) {
        getInput(results);
        printf(results);
        if (results[0] == '\n') {
            int x = fork();
            if (x == 0) {
                printf("Child\n");
                exec("/BIN/LS");
            }
            else {
                printf("Parent\n");
                yield();
                printf("back from yield\n");
            }
            made = true;
            printf("> ");
        }
    }
}