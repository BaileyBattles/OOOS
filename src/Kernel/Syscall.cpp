#include "CPU/InterruptManager.h"
#include "Util/String.h"

#include "Kernel/Syscall.h"

void SyscallHandler::initialize() {
    registerISRHandler(this, 80);
}
void SyscallHandler::handleInterrupt(registers_t r){
    kprint("We're getting to it...");
}

void call_syscall(void *args,  void *results) {
    __asm__ __volatile__("movl %%eax, %%ebx" ::"a"((u32)args)
                : "memory");
    __asm__ __volatile__("movl %%eax, %%ecx" ::"a"((u32)results)
                : "memory");
    __asm__ __volatile__("int $80");
    kprint("now we're here\n");
    u32 val;
    __asm__("movl %%eax,%0" : "=r"(val));
}

extern "C"
void handle_syscall(void *args, void *results) {
    kprint((const char*)args);
    ((char*)results)[0] = 'r';
    ((char*)results)[1] = 'e';
    ((char*)results)[2] = 't';
    ((char*)results)[3] = 'u';
    ((char*)results)[4] = 'r';
    ((char*)results)[5] = 'n';
    ((char*)results)[6] = '!';
    ((char*)results)[7] = '\n';
    ((char*)results)[8] = '\0';
}