#ifndef __MEMCPY_H__
#define __MEMCPY_H__

#include "kernel/types.h"

void memory_copy(char *source, char *dest, int nbytes);
void memory_set(u8 *dest, u8 val, u32 len);

#endif

