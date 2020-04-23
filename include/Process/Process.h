#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "Kernel/Types.h"
#include "Memory/Paging.h"
#include "Process/ELFLoader.h"

#define MAX_NUM_PROCESSES 100

class Process {
public:
    Process(const char path[]);
    void exec();

    PagingStructure *getPagingStructure();
private:
    char *path;
	ELFLoader elfLoader;
    
    PagingStructure pagingStructure;
    int loadElf(const char path[]);
};


#endif