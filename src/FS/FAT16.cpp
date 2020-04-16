#include "Drivers/Screen.h"
#include "FS/FAT16.h"
#include "Util/Memcpy.h"
#include "Util/String.h"

FAT16::FAT16(FileDevice &theFileDevice) :
    FileSystem(theFileDevice) 
{
    numClusters = fileDevice->deviceSize() / (SECTORS_PER_CLUSTER * FAT16_SECTOR_SIZE);
    if (numClusters > FAT16_MAX_CLUSTERS)
        numClusters = FAT16_MAX_CLUSTERS;
    //Eventually we won't want to format it every time
    format();
}

void FAT16::format() {
    char zeroSector[FAT16_SECTOR_SIZE];
    memory_set(zeroSector, '\0', FAT16_SECTOR_SIZE);
    int i;
    char buff[10];

    for (i = 0; i < numClusters * SECTORS_PER_CLUSTER; i++) {
        int status = fileDevice->writeSector(i, zeroSector, FAT16_SECTOR_SIZE);
        if (status == -1) {
            kprint("EXT2.cpp:Failed to write sector\n");
        }
    }
    
    writeBPB();
    writeFATs();
}

void FAT16::writeBPB() {
    memory_set(BPB.reserved, '\0', FAT16_SECTOR_SIZE);
    fileDevice->writeSector(FAT16_BPB_SECTOR, (char*)&BPB, FAT16_SECTOR_SIZE);

}

void FAT16::writeFATs() {
    
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