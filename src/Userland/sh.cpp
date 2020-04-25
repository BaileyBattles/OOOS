#include "Kernel/Syscall.h"

int main() {
    char data[] = "From the shell!\n";
    char results[] = " ";
    printf(data);
    results[0] = '\0';
    while (true) {
        getInput(results);
        printf(results);
    }
    //printf(results);
}