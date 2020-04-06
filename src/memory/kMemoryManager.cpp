#include "kernel/types.h"
#include "memory/kMemoryManger.h"
#include "memory/paging.h"
#include "util/memcpy.h"

////////////////////
// kMemoryManager //
////////////////////

KMemoryManager::KMemoryManager(u32 address){
    pagemallocStartAddress = calculateNextAllignedAddress(address, PAGE_SIZE);
    kmallocStartAddress = pagemallocStartAddress + PAGE_MEMORY_SIZE;
    memory_set((void*)kmallocMap, 0, KERNEL_HEAP_SIZE / 8);
    kmallocBitmapLength = KERNEL_HEAP_SIZE / 8;
}

void *KMemoryManager::kmalloc(int numBytes){
    u32 startIndex = findNFree(numBytes, kmallocMap, kmallocBitmapLength);
    if (startIndex == -1){
        return nullptr;
    }
    setChunkUsed(startIndex, numBytes, kmallocMap);
    return kmallocStartAddress + startIndex;
}

void *KMemoryManager::calculateNextAllignedAddress(u32 address, u32 pageSize){
    if ((pageSize - 1) & address){
        return (void *)((address+pageSize) & ~(pageSize-1));
    }
    return (void *)address;
}

/*
Since each bit labels a byte of memory, 
check and see if that byte is free
*/
bool KMemoryManager::indexFree(u32 index, u8 bitmap[]) {
    int byte = index / 8;
    int offset = index % 8;
    int bit = (bitmap[byte] >> (7 - offset)) & 0x1;
    if (bit == 0)
        return true;
    return false;
}

/*
Return the index of the chunk in kMemory with
n free in a row
*/
u32 KMemoryManager::findNFree(int numBytes, u8 bitmap[], int bitmapLength){
    u32 numInARow = 0;
    u32 start = 0;
    for (int i = 0; i < bitmapLength; i++){
        if (numInARow == numBytes)
            return start;
        if (indexFree(i, bitmap)){
            numInARow += 1;
        }
        else {
            numInARow = 0;
            start = i + 1;
        }  
    }
    return -1;
}

void KMemoryManager::setIndexUsed(u32 index, u8 bitmap[]){
    int byte = index / 8;
    int offset = index % 8;
    bitmap[byte] = (bitmap[byte] | 0x1 << (7 - offset));
}

//Index is the bit number, i.e index of 2 is the third bit
void KMemoryManager::setChunkUsed(u32 index, int numBytes, u8 bitmap[]){
    for (int i = index; i < index + numBytes; i++){
        setIndexUsed(i, bitmap);
    }
}