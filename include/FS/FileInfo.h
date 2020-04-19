#ifndef __FILEINFO_H__
#define __FILEINFO_H__

#include "Kernel/Types.h"

typedef struct {
    u32 size; //in bytes
    u32 startSector;
    //And there will be lots more
} FileInfo;

#endif