#include "Kernel/Syscall.h"

int main() {
    const char data[] = "From the shell!\n";
    char results[] = " ";
    printf((char*)data);
    results[0] = '\0';
    int x = printf("> ");
    if (x == 100) {
        printf("Did it\n");
    }
    while (true) {
        getInput(results);
        printf(results);
        if (results[0] == '\n')
            printf("> ");
    }
}