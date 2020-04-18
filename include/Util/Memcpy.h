#ifndef __MEMCPY_H__
#define __MEMCPY_H__

#include "Kernel/Types.h"

void memory_copy(const char *source, char *dest, int nbytes);
void memory_set(void *dest, u8 val, u32 len);

#endif

