#include "Kernel/Syscall.h"

int main() {
    char data[] = "From the shell!\n";
    call_syscall((void*)data);
}