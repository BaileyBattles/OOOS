#include "Kernel/Syscall.h"

int main() {
    char data[] = "From the shell!\n";
    char results[] = "From the shell!\n";
    printf(data);
    getInput(results);
    printf(results);
}