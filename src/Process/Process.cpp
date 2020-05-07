#include "CPU/Gdt.h"
#include "Drivers/Keyboard.h"
#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Process/Process.h"
#include "Process/Scheduler.h"
#include "Util/String.h"

extern "C" void enteruser(u32 entryPoint);

Process Process::createInitProcess(void (*func)(Process *)) {
    Process *initProcess = (Process*)KMM.kmalloc(sizeof(Process));
    memory_set(initProcess, '\0', sizeof(Process));
    initProcess->pcb.eip = (u32)func;
    initProcess->pagingStructure = PageTableManager::the().getCurrentPagingStructure();
    Scheduler::the().scheduleProcess(initProcess);
    //func(initProcess);
}

Process Process::createChildProcess(bool user) {
    Process childProcess;
    childProcess.pagingStructure = PageTableManager::the().initializeProcessPageTable();
    pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;
    childProcess.isUserMode = user;
    childProcess.socket = this->socket;
    return childProcess;
}

void Process::connectToKeyboard(Keyboard *keyboard) {
    if (keyboard != nullptr)
        keyboard->registerTerminal(this);
}

void Process::run() {
    PageTableManager::the().pageTableSwitch(this);
    if (isUserMode) {
        enterUserMode(pcb.eip, parent->pcb);
    }
    else {
        ((void (*)(Process*))pcb.eip)(this);
    }
}

IPCSocket *Process::theSocket() {
    return &socket;
}

void Process::readFromIPC() {
    while (true) {
        char c1;
        int status = socket.read(&c1, 1);
        if (status == 0) {
            char buff[2];
            buff[0] = c1;
            buff[1] = '\0';
            kprint(buff);
        }
    }
}


PagingStructure* Process::getPagingStructure() {
    return &pagingStructure;
}

void Process::exec(const char path[]) {
    path = "/BIN/SH";
    PCB oldPcb;
    storeRegisters(oldPcb);
    char *copiedPath = (char*)KMM.kmalloc(strlen(path) + 1);
    strcpy(path, copiedPath);
    PageTableManager::the().pageTableSwitch(this);
    PageTableManager::the().mmap((void*)USERSPACE_START_VIRTUAL, TOTAL_MEMORY / 8);
    ELFInfo elfInfo = elfLoader.load(copiedPath);

    pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;
    pcb.eip = elfInfo.entryAddress;

    asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
            : "memory");
}

void Process::storeRegisters(PCB &processControlBlock) {
    asm("\t movl %%esp,%0" : "=r"(processControlBlock.esp));
    //asm("\t movl %%ebp,%0" : "=r"(processControlBlock.ebp));
}

void Process::enterUserMode(u32 entryAddress, PCB &pcb) {
    u32 val;
    __asm__("movl %%ebp,%0" : "=r"(val));
    pcb.eip = *(u32*)(val + 4);
    __asm__("movl %%ebp,%0" : "=r"(pcb.esp));
    
    __asm__("movl %%esp,%0" : "=r"(val));
    setTSSStack(val);
    asm volatile("movl %%eax, %%esp" ::"a"(USERSPACE_START_VIRTUAL + 0x1000)
                : "memory");
    enteruser(entryAddress);
}