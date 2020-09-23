#include "Memory/KMemoryManager.h"
#include "Process/Scheduler.h"
#include "Drivers/Screen.h"

extern "C" u32 read_eip();

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
}

void Scheduler::yield() {
    int x = 42;
    currentProcess->pcb.eip = read_eip();
    if (currentProcess->pcb.yieldFlag == 0) {
        currentProcess->pcb.yieldFlag = 1;
        asm("\t movl %%esp,%0" : "=r"(currentProcess->pcb.esp));
        asm("\t movl %%ebp,%0" : "=r"(currentProcess->pcb.ebp));
        runNext();
    }
    currentProcess->pcb.yieldFlag = 0;
    currentProcess->reloadShell();
}