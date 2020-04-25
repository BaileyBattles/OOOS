#include "CPU/InterruptManager.h"
#include "Process/Process.h"
#include "Util/String.h"
#include "Kernel/Syscall.h"

int call_syscall(int num, void *args,  void *results) {
    __asm__ __volatile__("movl %%eax, %%ebx" ::"a"((u32)args)
                : "memory");
    __asm__ __volatile__("movl %%eax, %%ecx" ::"a"((u32)results)
                : "memory");
    __asm__ __volatile__("movl %%eax, %%edx" ::"a"((u32)num)
                : "memory");
    __asm__ __volatile__("int $80");
}

extern "C"
int handle_syscall(int num, void *args, void *results) {
    return _syscall_table[num]((char*)args, (char*)results);
}

int printf(char *buffer) {
    return call_syscall(0, (void*)buffer, nullptr);
}

int getInput(char *buffer) {
    return call_syscall(1, nullptr, (void*)buffer);
}

//Syscall Wrappers
int _kprint(char* buffer, char* result){
    kprint(buffer);
    return 100;
}

int _getInput(char* buffer, char* result) {
    result[0] = '\0';
    Process::currentProcess->theSocket()->read(result, 1);
    return 0;
}