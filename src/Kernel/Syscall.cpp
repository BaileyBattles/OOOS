#include "CPU/InterruptManager.h"
#include "Process/Process.h"
#include "Util/String.h"
#include "Kernel/Syscall.h"

void call_syscall(int num, void *args,  void *results) {
    __asm__ __volatile__("movl %%eax, %%ebx" ::"a"((u32)args)
                : "memory");
    __asm__ __volatile__("movl %%eax, %%ecx" ::"a"((u32)results)
                : "memory");
    __asm__ __volatile__("movl %%eax, %%edx" ::"a"((u32)num)
                : "memory");
    __asm__ __volatile__("int $80");
}

extern "C"
void handle_syscall(int num, void *args, void *results) {
    _syscall_table[num]((char*)args, (char*)results);
}

void printf(char *buffer) {
    call_syscall(0, (void*)buffer, nullptr);
}

void getInput(char *buffer) {
    call_syscall(1, nullptr, (void*)buffer);
}

void theRealGetInput(char *buffer) {
    buffer[0] = 'a';
    buffer[1] = 'b';    
    buffer[2] = 'c';
    buffer[3] = '\n';
    buffer[4] = '\0';
}

//Syscall Wrappers
void _kprint(char* buffer, char* result){
    kprint(buffer);
}

void _getInput(char* buffer, char* result) {
    result[0] = '\0';
    Process::currentProcess->theSocket()->read(result, 1);
}