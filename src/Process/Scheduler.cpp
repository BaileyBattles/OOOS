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

void Scheduler::exit() {
    for (int i = 0; i < numProcesses - 1; i++) {
        processQueue[i] = processQueue[i + 1];
    }
    numProcesses--;
    currentProcess = processQueue[0];
    currentProcess->run();
}

void Scheduler::yield() {
    
}