#include "Kernel/Syscall.h"

int main() {
    const char data[] = "From the shell!\n";
    char results[] = " ";
    printf((char*)data);
    results[0] = '\0';
    printf("> ");
    while (true) {
        getInput(results);
        printf(results);
        if (results[0] == '\n')
            printf("> ");
    }
}