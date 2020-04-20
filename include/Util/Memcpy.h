#ifndef __MEMCPY_H__
#define __MEMCPY_H__

#include "Kernel/Types.h"

void memory_copy(const char *source, char *dest, int nbytes);
void memory_set(void *dest, u8 val, u32 len);
//if buff1 == buff2 up to nbytes returns 0
//if buff1 < buff2 it returns -1
//if buff1 > buff2 it retunrs 1 
int memory_cmp(const void *buff1, const void *buff2, int nbytes);

#endif

