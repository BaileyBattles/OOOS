#include "memory/kMemoryManger.h"
#include "util/memcpy.h"

////////////////////
// kMemoryManager //
////////////////////

kMemoryManager::kMemoryManager(void *startAdress){
    startAddress = startAddress;
    memory_set((void*)memoryMap, 0, KERNEL_HEAP_SIZE);
    bitmapLength = KERNEL_HEAP_SIZE / 8;
}

void *kMemoryManager::kmalloc(int numBytes){
    u32 startIndex = findNFree(numBytes);
    return startAddress + startIndex;
}

/*
Since each bit labels a byte of memory, 
check and see if that byte is free
*/
bool kMemoryManager::byteFree(u32 index) {
    for (int i = 0; i < bitmapLength; i++) {
        if (i*8 + 8 > index){
            int offset = index - i*8;
            int bit = (memoryMap[i] >> (7 - offset)) & 0x1;
            if (bit == 0)
                return true;
            return false;
        }
    }
    return false;
}

/*
Return the index of the chunk in kMemory with
n free in a row
*/
u32 kMemoryManager::findNFree(int numBytes){
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