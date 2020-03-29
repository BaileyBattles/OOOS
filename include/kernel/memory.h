#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "kernel/types.h"

class MemoryManager{
public:
    MemoryManager();
    void* malloc();

private:
    u32 start;
    int length;
    int* map;
};

#endif