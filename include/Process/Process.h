#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "Memory/Paging.h"


class Process {
public:
    Process(const char path[]);
    void exec();

    const PagingStructure *getPagingStructure();
private:
    PagingStructure pagingStructure;
};


#endif