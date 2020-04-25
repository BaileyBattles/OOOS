#include "Kernel/SyscallStubs.h"
#include "Kernel/Types.h"

int call_syscall(int num, void *args,  void *results) {
    __asm__ __volatile__("movl %%eax, %%ebx" ::"a"((u32)args)
                : "memory");
    __asm__ __volatile__("movl %%eax, %%ecx" ::"a"((u32)results)
                : "memory");
    __asm__ __volatile__("movl %%eax, %%edx" ::"a"((u32)num)
                : "memory");
    __asm__ __volatile__("int $80");
}

int printf(char *buffer) {
    return call_syscall(0, (void*)buffer, nullptr);
}

int getInput(char *buffer) {
    return call_syscall(1, nullptr, (void*)buffer);
}

