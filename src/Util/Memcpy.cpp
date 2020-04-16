#include "Util/Memcpy.h"

void memory_copy(char *source, char *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(void *dest, u8 val, u32 len) {
    u8 *destCast = (u8 *)dest;
    for (int i = 0; i < len; i++){
        destCast[i] = val;
    }
}