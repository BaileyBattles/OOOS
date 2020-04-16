#include "FS/FAT16.h"
#include "Util/Memcpy.h"

FAT16::FAT16(FileDevice &theFileDevice) :
    FileSystem(theFileDevice) 
{
    //Eventually we won't want to format it every time
    format();
}

void FAT16::writeBPB() {
    memory_set(BPB.reserved, '\0', 512);
}

void FAT16::format() {
    writeBPB();
    fileDevice->writeSector(FAT16_BPB_SECTOR, (char*)&BPB, FAT16_SECTOR_SIZE);
}

bool FAT16::isArchive(FAT16_DirEnt dirEnt){
    return dirEnt.attribute & (1 << 5);
}
bool FAT16::isDir(FAT16_DirEnt dirEnt){
    return dirEnt.attribute & (1 << 4);
}
bool FAT16::isVolumeID(FAT16_DirEnt dirEnt){
    return dirEnt.attribute & (1 << 3);
}
bool FAT16::system(FAT16_DirEnt dirEnt){
    return dirEnt.attribute & (1 << 2);
}
bool FAT16::isHidden(FAT16_DirEnt dirEnt){
    return dirEnt.attribute & (1 << 1);
}
bool FAT16::isReadOnly(FAT16_DirEnt dirEnt){
    return dirEnt.attribute & 1;
}