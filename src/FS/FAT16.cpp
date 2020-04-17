#include "Drivers/Screen.h"
#include "FS/FAT16.h"
#include "Util/Memcpy.h"
#include "Util/String.h"

FAT16::FAT16(FileDevice &theFileDevice) :
    FileSystem(theFileDevice) 
{
    numEntriesPerSector = FAT16_SECTOR_SIZE / (sizeof(FATEntry) * 8);
    u32 numEntriesPerCluster = numEntriesPerSector * SECTORS_PER_CLUSTER;
    numClusters = fileDevice->deviceSize() / (SECTORS_PER_CLUSTER * FAT16_SECTOR_SIZE);
    numFATClusters = numClusters / numEntriesPerCluster;

    rootCluster = numFATClusters + 1;

    startFAT = SECTORS_PER_CLUSTER * 1; //start in the second cluster
    if (numClusters > FAT16_MAX_CLUSTERS)
        numClusters = FAT16_MAX_CLUSTERS;
    //Eventually we won't want to format it every time
    format(false);
}


void FAT16::format(bool eraseData) {
    char zeroSector[FAT16_SECTOR_SIZE];
    memory_set(zeroSector, '\0', FAT16_SECTOR_SIZE);
    int i;
    char buff[10];

    if (eraseData) {
        for (i = 0; i < numClusters * SECTORS_PER_CLUSTER; i++) {
            int status = fileDevice->writeSector(i, zeroSector, FAT16_SECTOR_SIZE);
            if (status == -1) {
                kprint("FAT16.cpp:Failed to write sector\n");
            }
        }
    }
    
    writeBPB();
    //writeFAT();
    createRootDir();
    ls(rootCluster);
}

//////////////////////
// Format Functions //
//////////////////////

void FAT16::writeBPB() {
    memory_set(BPB.reserved, '\0', FAT16_SECTOR_SIZE);
    fileDevice->writeSector(FAT16_BPB_SECTOR, (char*)&BPB, FAT16_SECTOR_SIZE);
}

void FAT16::writeFAT() {
    setFATEntry(FAT16_RESERVED_CLUSTER, 0);
    for (int i = 0; i < numFATClusters; i++) {
        setFATEntry(FAT16_RESERVED_CLUSTER, i + 1);
    }   
}

void FAT16::createRootDir() {
    FAT16_DirEnt dot = makeDir(".", 1, rootCluster, rootCluster);  
    FAT16_DirEnt doubleDot = makeDir("..", 2, rootCluster, rootCluster);
}


////////////////////////
// Core Functionality //
////////////////////////

FAT16_DirEnt FAT16::makeDir(char fileName[], int nameLen, 
                            int startCluster, int homeCluster) {
    FAT16_DirEnt dir;
    memory_set(&dir, '\0', sizeof(FAT16_DirEnt));
    if (nameLen > 8) {
        kprint("FAT16:Name can only be 8 chars");
        return dir;
    }

    for (int i = 0; i < nameLen; i++)
        dir.fileName[i] = fileName[i];
    
    setDir(dir);

    dir.startingCluster = startCluster;
    writeDirEntToSector(dir, homeCluster);
    return dir;
}

void FAT16::ls(int homeCluster) {
    for (int sector = 0; sector < SECTORS_PER_CLUSTER; sector++) {
        char FATSector[FAT16_SECTOR_SIZE];
        readSector(homeCluster, sector, FATSector);
        for (int i = 0; i < FAT16_SECTOR_SIZE; i += sizeof(FAT16_DirEnt)) {
            FAT16_DirEnt entry;
            memory_copy(FATSector + i, (char *)&entry, sizeof(FAT16_DirEnt));
            if (entry.fileName[0] != '\0') {
                kprint(entry.fileName);
                kprint("\n");
            }
        }
    }
}

//////////////////////////
// FileDevice Interface //
//////////////////////////

// Read sector number clusterNum * SECTORS_PER_CLUSTER + sectorOffset
int FAT16::readSector(u32 clusterNum, u32 sectorOffset, char FATSector[]){
    u32 sector = clusterNum * SECTORS_PER_CLUSTER + sectorOffset;
    return fileDevice->readSector(sector, FATSector, FAT16_SECTOR_SIZE);
}

// Write sector number clusterNum * SECTORS_PER_CLUSTER + sectorOffset
int FAT16::writeSector(u32 clusterNum, u32 sectorOffset, char FATSector[]){
    u32 sector = clusterNum * SECTORS_PER_CLUSTER + sectorOffset;
    return fileDevice->writeSector(sector, FATSector, FAT16_SECTOR_SIZE);
}

void FAT16::setFATEntry(FATEntry entry, u32 index){
    u32 sectorNum = (index / numEntriesPerSector) + startFAT;
    u32 offset = (index % numEntriesPerSector)*4;
    char FATSector[FAT16_SECTOR_SIZE];
    fileDevice->readSector(sectorNum, FATSector, FAT16_SECTOR_SIZE);
    setFourBytes(entry, FATSector, offset);
    fileDevice->writeSector(sectorNum, FATSector, FAT16_SECTOR_SIZE);
}   


void FAT16::writeDirEntToSector(FAT16_DirEnt dirEnt, u32 clusterNum) {
    char FATSector[FAT16_SECTOR_SIZE];
    int offset = -1;
    for (int sector = 0; sector < SECTORS_PER_CLUSTER; sector++) {
        readSector(clusterNum, sector, FATSector);
        offset = getSectorOffsetForDirEnt(FATSector);
        if (offset != -1){
            memory_copy((char*)&dirEnt, FATSector + offset, sizeof(FAT16_DirEnt));
            writeSector(clusterNum, sector, FATSector);
            return;
        }
    }
    if (offset == -1) {
        kprint("FAT16:Cluster Full!  Can't make file");
    }
}

//////////////////////
// Helper Functions //
//////////////////////

void FAT16::setFourBytes(u32 value, char buffer[], u32 offset) {
    buffer[offset] = (value >> 24) & 0xFF;
    buffer[offset + 1] = (value >> 16) & 0xFF;
    buffer[offset + 2] = (value >> 8) & 0xFF;
    buffer[offset + 3] = value & 0xFF;
}



u32 FAT16::getSectorOffsetForDirEnt(char FATSector[]) {
    for (int i = 0; i < FAT16_SECTOR_SIZE; i += sizeof(FAT16_DirEnt)) {
        bool isSpace = true;
        for (int j = 0; j < sizeof(FAT16_DirEnt); j++) {
            if (FATSector[i + j] != '\0')
                isSpace = false;
        }
        if (isSpace)
            return i;
    }
    return 0;
}


/////////////////////////
// DirEnt Manipulation //
/////////////////////////

bool FAT16::isArchive(FAT16_DirEnt dirEnt) {
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

void FAT16::setArchive(FAT16_DirEnt &dirEnt) {
    dirEnt.attribute |= (1 << 5);
}
void FAT16::setDir(FAT16_DirEnt &dirEnt){
    dirEnt.attribute |= (1 << 4);
}
void FAT16::setVolumeID(FAT16_DirEnt &dirEnt){
    dirEnt.attribute |= (1 << 3);
}
void FAT16::setSystem(FAT16_DirEnt &dirEnt){
    dirEnt.attribute |= (1 << 2);
}
void FAT16::setHidden(FAT16_DirEnt &dirEnt){
    dirEnt.attribute |= (1 << 1);
}
void FAT16::setReadOnly(FAT16_DirEnt &dirEnt){
    dirEnt.attribute |= 1;
}