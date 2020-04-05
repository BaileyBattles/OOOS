#ifndef __ALLOCATE_H__
#define __ALLOCATE_H__

#include "kernel/types.h"

#define KERNEL_HEAP_SIZE 10*KB

class KMemoryManager{
public:
    KMemoryManager(u32 startAddress);
    void *kmalloc(int numBytes);
private:
    u32 baseAddress;
    u8 memoryMap[KERNEL_HEAP_SIZE / 8];
    int bitmapLength;
    void *startAddress;

    bool byteFree(u32 index);
    u32 findNFree(int numBytes);
    void setChunkUsed(u32 index, int numBytes);
    void setByteUsed(u32 index);
};

#endif