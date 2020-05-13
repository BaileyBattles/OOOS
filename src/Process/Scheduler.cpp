#include "Memory/KMemoryManager.h"
#include "Process/Scheduler.h"

extern "C" u32 get_eip();

Scheduler::Scheduler() {
    numProcesses = 0;
}

void Scheduler::scheduleProcess(Process* process) {
    processQueue[numProcesses] = process;
    numProcesses++;
}

Process* Scheduler::runningProcess() {
    return currentProcess;
}

void Scheduler::runNext() {
    // u32 physical = PageTableManager::the().getPhysicalAddress((u32)&Scheduler::the());
    Process *theProcess = processQueue[0];
    for (int i = 0; i < numProcesses - 1; i++) {
        processQueue[i] = processQueue[i + 1];
    }
    processQueue[numProcesses - 1] = theProcess;
    currentProcess = processQueue[0];
    currentProcess->run();
}

void Scheduler::removeProcess(Process *process) {
    int i;  
    int processNumber = -1;
    for (i = 0; i < numProcesses; i++) {
        if (process->getPID() == processQueue[i]->getPID())
            processNumber = i;
    }

    if (processNumber == -1)
        return;

    for (int i = processNumber; i < numProcesses - 1; i++) {
        processQueue[i] = processQueue[i + 1];
    }
    numProcesses--;

    if (processNumber == 0) {
        currentProcess = processQueue[0];
        currentProcess->run();
    }
}

void Scheduler::yield() {
    int currentPID = currentProcess->getPID();
    u32 ebp; u32 esp;
    asm("\t movl %%esp,%0" : "=r"(currentProcess->pcb.esp));
    asm("\t movl %%ebp,%0" : "=r"(currentProcess->pcb.ebp));
    currentProcess->pcb.eip = get_eip();
    if (currentProcess->getPID() == currentPID)
        runNext();
}