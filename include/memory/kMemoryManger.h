#ifndef __ALLOCATE_H__
#define __ALLOCATE_H__

#include "kernel/types.h"

#define PAGE_MEMORY_SIZE 32 * 1024
#define KERNEL_HEAP_SIZE 10*KB

class KMemoryManager{
public:
    KMemoryManager(u32 startAddress);
    void *kmalloc(int numBytes);
    void *pagemalloc(); //allocate one page
private:
    u32 baseAddress;
    u8 kmallocMap[KERNEL_HEAP_SIZE / 8];
    int kmallocBitmapLength;
    void *pagemallocStartAddress;
    void *kmallocStartAddress;

    //Bitmap Functions
    //
    //Can't make it it's own class since we would
    //need to be able to allocate memory for that
    bool indexFree(u32 index, u8 bitmap[]);
    u32 findNFree(int numBytes, u8 bitmap[], int bitmapLength);
    void setChunkUsed(u32 index, int numBytes, u8 bitmap[]);
    void setIndexUsed(u32 index, u8 bitmap[]);
    
    void *calculateNextAllignedAddress(u32 address, u32 pageSize);
};

#endif