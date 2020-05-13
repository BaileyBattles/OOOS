#include "CPU/Gdt.h"
#include "Drivers/Keyboard.h"
#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Process/Process.h"
#include "Process/Scheduler.h"
#include "Util/String.h"

extern "C" void enteruser(u32 entryPoint);
extern "C" u32 get_eip();
extern "C" void jumpToCode(u32 eip, u32 esp, u32 ebp);

int Process::nextPID = 1;
int Process::getNextPID() {
    nextPID++;
    return nextPID - 1;
}

Process processes[MAX_NUM_PROCESSES];

Process Process::createInitProcess(void (*func)(Process *)) {
    Process *initProcess = &processes[0];
    memory_set(initProcess, '\0', sizeof(Process));
    initProcess->pid = 0;
    initProcess->pcb.esp = 0;
    initProcess->pcb.ebp = 0;
    initProcess->pcb.eip = (u32)func;
    initProcess->pagingStructure = PageTableManager::the().getCurrentPagingStructure();
    Scheduler::the().scheduleProcess(initProcess);
    //func(initProcess);
}

Process *Process::createChildProcess(bool user) {
    int newPid = getNextPID();
    Process *childProcess = &processes[newPid];
    childProcess->pagingStructure = PageTableManager::the().initializeProcessPageTable();
    childProcess->pid = newPid;
    PageTableManager::the().pageTableSwitch(childProcess);
    PageTableManager::the().mmap((void*)USERSPACE_START_VIRTUAL, TOTAL_MEMORY / 8);
    PageTableManager::the().pageTableSwitch(this);

    if (isUserMode) {
        childProcess->pcb.esp = USERSPACE_START_VIRTUAL + 0x1000;
        childProcess->pcb.ebp = USERSPACE_START_VIRTUAL + 0x1000;
    }
    childProcess->isUserMode = user;
    childProcess->socket = this->socket;
    return childProcess;
}

int Process::getPID() {
    return pid;
}

int Process::fork() {
    Process *newProcess = createChildProcess(true);
    pcb.forkFlag = 0;
    newProcess->pcb.forkFlag = 0x10987;
    Scheduler::the().scheduleProcess(newProcess);
    newProcess->pcb.eip = get_eip();

    if (newProcess != Scheduler::the().runningProcess()) {
        //parent
        asm("\t movl %%esp,%0" : "=r"(newProcess->pcb.esp));
        asm("\t movl %%ebp,%0" : "=r"(newProcess->pcb.ebp));
        PageTableManager::the().copyMemory(this->getPagingStructure(), 
                    newProcess->getPagingStructure());
        return newProcess->getPID();
    }
    else {
        //child
        //this == newProcess so the regs we want are in pcb
        // asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
        //     : "memory");
        // asm volatile("movl %%eax, %%ebp" ::"a"(pcb.ebp)
        //     : "memory");
        return 0;
    }
}

void Process::connectToKeyboard(Keyboard *keyboard) {
    if (keyboard != nullptr)
        keyboard->registerTerminal(this);
}

void Process::run() {
    PageTableManager::the().pageTableSwitch(this);

    if (pcb.esp == 0 && pcb.ebp == 0) 
        ((void (*)(Process*))pcb.eip)(this);


    if (pcb.eip > USERSPACE_START_VIRTUAL) {
        enterUserMode(pcb.eip, pcb);
    }
    else {  
        //((void (*)(Process*))pcb.eip)(this);
        u32 val;   
        __asm__("movl %%esp,%0" : "=r"(val));
        setTSSStack(val);
        jumpToCode(pcb.eip, pcb.esp, pcb.ebp);

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
    __asm__("movl %%esp,%0" : "=r"(val));
    setTSSStack(val);

    //need to move entery address to register since we move the stack
    asm volatile("movl %%eax, %%ebx" ::"a"(entryAddress)
                : "memory");
    asm volatile("movl %%eax, %%esp" ::"a"(pcb.esp)
                : "memory");
    asm volatile("movl %%eax, %%ebp" ::"a"(pcb.ebp)
                : "memory");
    __asm__("movl %%ebx,%0" : "=r"(entryAddress));

    enteruser(entryAddress);
}