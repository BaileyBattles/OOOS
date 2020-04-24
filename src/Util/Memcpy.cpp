#include "Memory/KMemoryManager.h"
#include "Util/Memcpy.h"
#include "Util/String.h"

void memory_copy(const void *source, void *dest, int nbytes) {
    int i;
    if ((u32)dest - (u32)source < nbytes) {
        u8 *temp = (u8*)KMM.kmalloc(nbytes);
        for (int i = 0; i < nbytes; i++) {
            *((u8*)temp + i) = *((u8*)source + i);
        }
        for (int i = 0; i < nbytes; i++) {
            *((u8*)dest + i) = *((u8*)temp + i);
        }
        return;
    }
    for (i = 0; i < nbytes; i++) {
        *((u8*)dest + i) = *((u8*)source + i);
    }
}

void memory_set(void *dest, u8 val, u32 len) {
    u8 *destCast = (u8 *)dest;
    for (int i = 0; i < len; i++){
        destCast[i] = val;
    }
}

int memory_cmp(const void *buff1, const void *buff2, int nbytes) {
    u8 *buff1Ptr = (u8*)buff1;
    u8 *buff2Ptr = (u8*)buff2;
    for (int i = 0; i < nbytes; i++) {
        if ((u8)buff1Ptr[i] < (u8)buff2Ptr[i])
            return -1;
        else if ((u8)buff1Ptr[i] > (u8)buff2Ptr[i])
            return 1;
    }
    return 0;
}
