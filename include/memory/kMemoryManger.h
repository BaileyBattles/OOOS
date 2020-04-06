#ifndef __ALLOCATE_H__
#define __ALLOCATE_H__

#include "kernel/types.h"
#include "memory/paging.h"

#define NUM_PAGES 1025
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
    void *kmallocStartAddress;

    u8 pagemallocMap[NUM_PAGES / 8];
    int pagemallocBitmapLength;
    void *pagemallocStartAddress;

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