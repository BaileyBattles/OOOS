#include "CPU/InterruptManager.h"
#include "Drivers/Screen.h"
#include "FS/VFS.h"
#include "Process/Process.h"
#include "Process/Scheduler.h"
#include "Util/String.h"
#include "Kernel/Syscall.h"

extern "C"
int handle_syscall(int num, void *args, void *results) {
    return _syscall_table[num]((char*)args, (char*)results);
}

//Syscall Wrappers
int _kprint(char* buffer, char* result){
    kprint(buffer);
    return 0;
}

int _getInput(char* buffer, char* result) {
    result[0] = '\0';
    Process* theProcess = Scheduler::the().runningProcess();
    if (theProcess->theSocket() != nullptr)
         theProcess->theSocket()->read(result, 1);
    return 0;
}

int _fork(char* buffer, char *result) {
    int val = Scheduler::the().runningProcess()->fork();
    //Scheduler::the().runNext();
    return val;
    //Scheduler::the().scheduleProcess(&newProcess);
    // Process::processQueue[0] = newProcess;
}

int _exec(char* buffer, char *result) {
    // Process::processQueue[0].exec(buffer);
}

int _ls(char* buffer, char *result) {
    VFS.ls("/BIN");
}

int _exit(char* buffer, char *result) {
    // asm volatile("movl %%eax, %%esp" ::"a"(0x10be68)
    //             : "memory");
    // ((void (*)(void))0x10562d)();

    while (true)
        ;
}

int _yield(char* buffer, char* retsult) {
    Scheduler::the().yield();
}