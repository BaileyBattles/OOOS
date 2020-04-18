#ifndef __ALLOCATE_H__
#define __ALLOCATE_H__

#include "Kernel/Types.h"
#include "Memory/Paging.h"

#define NUM_PAGES 1032 //Should be a multiple of 8 for simplicity
#define KERNEL_HEAP_SIZE 10*KB

#define KMALLOC_SIZE 1
#define PAGEMALLOC_SIZE 4*KB

#define BITS_IN_BYTE 8

#define KMM KMemoryManager::the()

class KMemoryManager{
public:
    static KMemoryManager& the()
    {
        static KMemoryManager instance; // Guaranteed to be destroyed.
                                // Instantiated on first use.
        return instance;
    }

    u32 theAddress() {
        static KMemoryManager instance;
        return (u32)&instance;
    }

    void initialize(u32 address);
    void *kmalloc(int numBytes);
    void kfree(void* buffer);
    void *pagemalloc(); //allocate one page
private:
    KMemoryManager(){};

    u32 baseAddress;

    u32 hackyFix();

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