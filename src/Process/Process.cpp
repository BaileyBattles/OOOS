#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Process/Process.h"
#include "Util/String.h"

extern "C" u32 get_eip();

Process Process::createInitProcess(void (*func)(Process *)) {
    Process *initProcess = (Process*)KMM.kmalloc(sizeof(Process));
    memory_set(initProcess, '\0', sizeof(Process));
    func(initProcess);
}

Process Process::createChildProcess(const char thePath[], int level) {
    Process childProcess;
    childProcess.pagingStructure = PageTableManager::the().initializeProcessPageTable();
    childProcess.path = (char *)KMM.kmalloc(strlen(thePath) + 1);
    memory_copy(thePath, childProcess.path, strlen(thePath) + 1);
    pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;

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
    ((void (*)(void))elfInfo.entryAddress)();
    asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
            : "memory");
}

void Process::storeRegisters(PCB &processControlBlock) {
    asm("\t movl %%esp,%0" : "=r"(processControlBlock.esp));
    //asm("\t movl %%ebp,%0" : "=r"(processControlBlock.ebp));
    processControlBlock.eip = get_eip();
}


// void oldExec() {
//     char *buff = (char*)0x1000000;
//     ELFInfo elfInfo = elfLoader.load(path, buff);

//     PageTableManager::the().pageTableSwitch(this);
//     u32 stackPointer;
//     u32 basePointer;
//     asm("\t movl %%esp,%0" : "=r"(stackPointer));
//     asm("\t movl %%ebp,%0" : "=r"(stackPointer));

//     switchRegisters(pcb);
//     ((void (*)(void))elfInfo.entryAddress)();
//     asm volatile("movl %%eax, %%esp" ::"a"(stackPointer)
//                  : "memory");
// }