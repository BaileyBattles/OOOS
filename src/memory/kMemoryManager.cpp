#include "kernel/types.h"
#include "memory/kMemoryManger.h"
#include "util/memcpy.h"

////////////////////
// kMemoryManager //
////////////////////

KMemoryManager::KMemoryManager(u32 address){
    startAddress = (void*)address;
    memory_set((void*)memoryMap, 0, KERNEL_HEAP_SIZE / 8);
    bitmapLength = KERNEL_HEAP_SIZE / 8;
}

void *KMemoryManager::kmalloc(int numBytes){
    u32 startIndex = findNFree(numBytes);
    if (startIndex == -1){
        return 0;
    }
    setChunkUsed(startIndex, numBytes);
    return startAddress + startIndex;
}

/*
Since each bit labels a byte of memory, 
check and see if that byte is free
*/
bool KMemoryManager::byteFree(u32 index) {
    for (int i = 0; i < bitmapLength; i++) {
        int byte = index / 8;
        int offset = index % 8;
        int bit = (memoryMap[byte] >> (7 - offset)) & 0x1;
        if (bit == 0)
            return true;
        return false;
    }
    return false;
}

/*
Return the index of the chunk in kMemory with
n free in a row
*/
u32 KMemoryManager::findNFree(int numBytes){
    u32 numInARow = 0;
    u32 start = 0;
    for (int i = 0; i < bitmapLength; i++){
        if (numInARow == numBytes)
            return start;
        if (byteFree(i)){
            numInARow += 1;
        }
        else {
            numInARow = 0;
            start = i + 1;
        }  
    }
    return -1;
}

void KMemoryManager::setByteUsed(u32 index){
    for (int i = 0; i < bitmapLength; i++) {
        int byte = index / 8;
        int offset = index % 8;
        memoryMap[byte] = (memoryMap[byte] | 0x1 << (7 - offset));
    }
}

//Index is the bit number, i.e index of 2 is the third bit
void KMemoryManager::setChunkUsed(u32 index, int numBytes){
    for (int i = index; i < index + numBytes; i++){
        setByteUsed(i);
    }
}