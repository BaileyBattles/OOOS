#include "Drivers/Screen.h"
#include "FS/FAT16.h"
#include "Util/Memcpy.h"
#include "Util/String.h"

FAT16::FAT16(FileDevice &theFileDevice) :
    FileSystem(theFileDevice) 
{
    numEntriesPerSector = FAT16_SECTOR_SIZE / (sizeof(FATEntry) * 8);
    numClusters = fileDevice->deviceSize() / (SECTORS_PER_CLUSTER * FAT16_SECTOR_SIZE);
    startFAT = SECTORS_PER_CLUSTER * 1; //start in the second cluster
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

    // for (i = 0; i < numClusters * SECTORS_PER_CLUSTER; i++) {
    //     int status = fileDevice->writeSector(i, zeroSector, FAT16_SECTOR_SIZE);
    //     if (status == -1) {
    //         kprint("EXT2.cpp:Failed to write sector\n");
    //     }
    // }
    
    writeBPB();
    writeFAT();
}

void FAT16::writeBPB() {
    memory_set(BPB.reserved, '\0', FAT16_SECTOR_SIZE);
    fileDevice->writeSector(FAT16_BPB_SECTOR, (char*)&BPB, FAT16_SECTOR_SIZE);
}

void FAT16::setFourBytes(u32 value, char buffer[], u32 offset) {
    buffer[offset] = (value >> 24) & 0xFF;
    buffer[offset + 1] = (value >> 16) & 0xFF;
    buffer[offset + 2] = (value >> 8) & 0xFF;
    buffer[offset + 3] = value & 0xFF;
}

void FAT16::setEntry(FATEntry entry, u32 index){
    u32 sectorNum = (index / numEntriesPerSector) + startFAT;
    u32 offset = (index % numEntriesPerSector)*4;
    char FATSector[FAT16_SECTOR_SIZE];
    fileDevice->readSector(sectorNum, FATSector, FAT16_SECTOR_SIZE);
    setFourBytes(entry, FATSector, offset);
    fileDevice->writeSector(sectorNum, FATSector, FAT16_SECTOR_SIZE);
}   

void FAT16::writeFAT() {
    setEntry(0xFFFFFFFF, 17);
    char FATSector[FAT16_SECTOR_SIZE];
    fileDevice->readSector(7, FATSector, 16);

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