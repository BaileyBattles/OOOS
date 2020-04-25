#include "CPU/InterruptManager.h"

#include "Kernel/Syscall.h"

void SyscallHandler::initialize() {
    registerISRHandler(this, 80);
}
void SyscallHandler::handleInterrupt(registers_t r){
    kprint("We're getting to it...");
}

void call_syscall(void *args) {
    __asm__ __volatile__("movl %%eax, %%ebx" ::"a"((u32)args)
                : "memory");
    __asm__ __volatile__("int $80");
}

extern "C"
void handle_syscall(void *args) {
    kprint((const char*)args);
}