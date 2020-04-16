#include "FS/FAT16.h"

FAT16::FAT16(FileDevice &theFileDevice) :
    FileSystem(theFileDevice) 
{
     for (int i = 0; i < 512; i++) {
        MBR.reserved[i] = 'a';
    }
    fileDevice->writeSector(FAT16_MBR_SECTOR, (char*)&MBR, FAT16_SECTOR_SIZE);
}