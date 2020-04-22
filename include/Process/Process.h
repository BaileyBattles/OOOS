#ifndef __PROCESS_H__
#define __PROCESS_H__

class Process {
public:
    Process(const char path[]);
    void exec();
};


#endif