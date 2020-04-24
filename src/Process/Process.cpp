#include "CPU/Gdt.h"
#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Process/Process.h"
#include "Util/String.h"

extern "C" void enteruser(u32 entryPoint);

Process Process::createInitProcess(void (*func)(Process *)) {
    Process *initProcess = (Process*)KMM.kmalloc(sizeof(Process));
    memory_set(initProcess, '\0', sizeof(Process));
    func(initProcess);
}

Process Process::createChildProcess(const char thePath[], int level, bool user) {
    Process childProcess;
    childProcess.pagingStructure = PageTableManager::the().initializeProcessPageTable();
    childProcess.path = (char *)KMM.kmalloc(strlen(thePath) + 1);
    memory_copy((const void*)thePath, (void*)childProcess.path, strlen(thePath) + 1);
    pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;
    childProcess.isUserMode = user;
    return childProcess;
}


PagingStructure* Process::getPagingStructure() {
    return &pagingStructure;
}

void Process::exec() {
    PCB oldPcb;
    storeRegisters(oldPcb);
    ELFInfo elfInfo = elfLoader.load(path);
    PageTableManager::the().pageTableSwitch(this);
    pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;
    asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
                : "memory");
    if (isUserMode) {
        enterUserMode(elfInfo.entryAddress);
    }
    else {
        ((void (*)(void))elfInfo.entryAddress)();
    }
    asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
            : "memory");
}

void Process::storeRegisters(PCB &processControlBlock) {
    asm("\t movl %%esp,%0" : "=r"(processControlBlock.esp));
    //asm("\t movl %%ebp,%0" : "=r"(processControlBlock.ebp));
}

void Process::enterUserMode(u32 entryAddress) {
    u32 val;
    __asm__("movl %%esp,%0" : "=r"(val));
    setTSSStack(val);
    enteruser(entryAddress);
}