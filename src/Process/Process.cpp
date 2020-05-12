#include "CPU/Gdt.h"
#include "Drivers/Keyboard.h"
#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Process/Process.h"
#include "Process/Scheduler.h"
#include "Util/String.h"

extern "C" void enteruser(u32 entryPoint);
extern "C" u32 get_eip();

int Process::nextPID = 1;
int Process::getNextPID() {
    nextPID++;
    return nextPID - 1;
}

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
    childProcess.pid = getNextPID();
    PageTableManager::the().pageTableSwitch(&childProcess);
    PageTableManager::the().mmap((void*)USERSPACE_START_VIRTUAL, TOTAL_MEMORY / 8);
    PageTableManager::the().pageTableSwitch(this);

    
    childProcess.pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;
    childProcess.isUserMode = user;
    childProcess.socket = this->socket;
    return childProcess;
}

int Process::fork() {
    Process newProcess = createChildProcess(true);
    pcb.eip = 0;
    Scheduler::the().scheduleProcess(&newProcess);
    newProcess.pcb.eip = get_eip();

    if (pcb.eip == 0) {
        asm("\t movl %%esp,%0" : "=r"(newProcess.pcb.esp));
        asm("\t movl %%ebp,%0" : "=r"(newProcess.pcb.ebp));
        PageTableManager::the().copyMemory(this->getPagingStructure(), 
                    newProcess.getPagingStructure());
        return 1;
    }
    else {
        //this == newProcess so the regs we want are in pcb
        asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
            : "memory");
        asm volatile("movl %%eax, %%ebp" ::"a"(pcb.ebp)
            : "memory");
        return 0;
    }
}

void Process::connectToKeyboard(Keyboard *keyboard) {
    if (keyboard != nullptr)
        keyboard->registerTerminal(this);
}

void Process::run() {

    PageTableManager::the().pageTableSwitch(this);

    // if (isUserMode) {
    //     enterUserMode(pcb.eip, parent->pcb);
    // }
    // else {  
        ((void (*)(Process*))pcb.eip)(this);
    //}
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
 
void Process::exit() {
    Scheduler::the().removeProcess(this);
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
    
    ELFInfo elfInfo = elfLoader.load(copiedPath);

    pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;
    pcb.eip = elfInfo.entryAddress;

    PageTableManager::the().pageTableSwitch(Scheduler::the().runningProcess());
    asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
            : "memory");
}

void Process::storeRegisters(PCB &processControlBlock) {
    asm("\t movl %%esp,%0" : "=r"(processControlBlock.esp));
    //asm("\t movl %%ebp,%0" : "=r"(processControlBlock.ebp));
}

void Process::enterUserMode(u32 entryAddress, PCB &pcb) {
    u32 val;
    // __asm__("movl %%ebp,%0" : "=r"(val));
    // pcb.eip = *(u32*)(val + 4);
    // __asm__("movl %%ebp,%0" : "=r"(pcb.esp));
    
    __asm__("movl %%esp,%0" : "=r"(val));
    setTSSStack(val);
    asm volatile("movl %%eax, %%esp" ::"a"(USERSPACE_START_VIRTUAL + 0x1000)
                : "memory");
    enteruser(entryAddress);
}