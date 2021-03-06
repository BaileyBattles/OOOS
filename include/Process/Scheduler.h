#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Process/Process.h"

#define MAX_NUM_PROCESSES 256

class Scheduler {
public:
    static Scheduler& the()
    {
        static Scheduler instance; // Guaranteed to be destroyed.
                                // Instantiated on first use.
        return instance;
    }
    void scheduleProcess(Process* process);
    Process* runningProcess();
    //Round robin for now
    void runNext();
    void removeProcess(Process *process);
    void yield();
private:
    Scheduler();

    Process* processQueue[MAX_NUM_PROCESSES];
    Process* currentProcess;
    int numProcesses;
};

#endif