#include "Kernel/Types.h"
#include "Memory/KMemoryManager.h"
#include "Memory/Paging.h"
#include "Util/Memcpy.h"
#include "Drivers/Screen.h"

////////////////////
// kMemoryManager //
////////////////////

extern "C" u32 e_stack;


void KMemoryManager::initialize(u32 address){
    endOfStaticMemory = (u32)calculateNextAllignedAddress(address, PAGE_SIZE);
    kmallocStartAddress = (void*)endOfStaticMemory;
    memory_set((void*)kmallocMap, 0, KERNEL_HEAP_SIZE / 8);
    kmallocBitmapLength = KERNEL_HEAP_SIZE / 8;

    pagemallocStartAddress = 0;
    memory_set((void*)pagemallocMap, 0, NUM_PAGES / 8);

    //set the pages used by the kernel as used

}

u32 KMemoryManager::endOfKernelToCopy() {
    //return (u32)calculateNextAllignedAddress((u32)&e_nonstatic, PAGE_SIZE);
}

int KMemoryManager::mallocKernelPages() {
    numKernelPages = ((u32)&e_stack + KERNEL_HEAP_SIZE) / (PAGE_SIZE) + 1;
    pagemallocBitmapLength = NUM_PAGES / 8;
        for (int i = 0; i < numKernelPages; i++) {
        setIndexUsed(i, pagemallocMap);
    }
}

void *KMemoryManager::kmalloc(int numBytes){
    u32 startIndex = findNFree(numBytes, kmallocMap, kmallocBitmapLength);
    if (startIndex == -1){
        return nullptr;
    }
    setChunkUsed(startIndex, numBytes, kmallocMap);
    return kmallocStartAddress + startIndex * KMALLOC_SIZE;
}

void KMemoryManager::kfree(void* buffer) 
{
}

void *KMemoryManager::pagemallocPhysical(int numPages){
    u32 startIndex = findNFree(numPages, pagemallocMap, pagemallocBitmapLength);
    if (startIndex == -1) {
        return nullptr;
    }
    setChunkUsed(startIndex, numPages, pagemallocMap);
    return pagemallocStartAddress + startIndex * PAGEMALLOC_SIZE;
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
        if (bitmap[i] != 0xFFFF) {
            for (int j = 0; j < BITS_IN_BYTE; j++) {
                int currentIndex = i * BITS_IN_BYTE + j;
                if (numInARow == numBytes)
                    return start;

                if (indexFree(currentIndex, bitmap)){
                    numInARow += 1;
                }
                else {
                    numInARow = 0;
                    start = currentIndex + 1;
                } 
            }
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