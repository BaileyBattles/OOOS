#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Process/Process.h"

#define INITIAL_PROCESS_QUEUE_SIZE 20

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
    void exit();
    void yield();
private:
    Scheduler();

    Process** processQueue;
    Process* currentProcess;
    int numProcesses;
};

#endif