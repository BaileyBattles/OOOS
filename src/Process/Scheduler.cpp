#include "Memory/KMemoryManager.h"
#include "Process/Scheduler.h"

Scheduler::Scheduler() {
    processQueue = (Process**)KMM.kmalloc(sizeof(Process*)*INITIAL_PROCESS_QUEUE_SIZE);
    numProcesses = 0;
}

void Scheduler::scheduleProcess(Process* process) {
    processQueue[numProcesses] = process;
    numProcesses++;
}

void Scheduler::runNext() {
    Process *nextProcess = processQueue[0];
    for (int i = 0; i < numProcesses - 1; i++) {
        processQueue[i] = processQueue[i + 1];
    }
    processQueue[numProcesses - 1] = nextProcess;
    nextProcess->run();
}