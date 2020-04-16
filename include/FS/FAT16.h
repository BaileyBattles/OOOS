#ifndef __FAT16_H__
#define __FAT16_H__

#include "Drivers/FileDevice.h"
#include "FS/FileSystem.h"

#define FAT16_MBR_SECTOR 0
#define FAT16_SECTOR_SIZE 512

class FAT16 : public FileSystem {
public:
    FAT16(FileDevice &fileDevice);
private:
    typedef struct {
        char reserved[512];
    } FAT16MBR;
    FAT16MBR MBR;
};

#endif