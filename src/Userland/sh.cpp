#include "Kernel/Syscall.h"

int main() {
    char data[] = "From the shell!\n";
    char results[] = "From the shell!\n";
    call_syscall((void*)data, (void*)results);
    call_syscall((void*)results, data);
}