#include "Drivers/Screen.h"
#include "FS/FAT16.h"
#include "Memory/KMemoryManager.h"
#include "Util/Memcpy.h"
#include "Util/String.h"

FAT16::FAT16(FileDevice &theFileDevice) :
    FileSystem(theFileDevice) 
{
    numEntriesPerSector = FAT16_SECTOR_SIZE / (sizeof(FATEntry) * 8);
    u32 numEntriesPerCluster = numEntriesPerSector * SECTORS_PER_CLUSTER;
    numClusters = fileDevice->deviceSize() / (SECTORS_PER_CLUSTER * FAT16_SECTOR_SIZE);
    numFATClusters = numClusters / numEntriesPerCluster;

    rootCluster = numFATClusters + 2;
    nextAvailableCluster = rootCluster + 2;
    memory_set(freedClusters, '\0', NUM_FREED_REMEMBERED);
    dirEntsPerSector = FAT16_SECTOR_SIZE / sizeof(FAT16_DirEnt);

    startFATSector = SECTORS_PER_CLUSTER * 1; //start in the second cluster
    if (numClusters > FAT16_MAX_CLUSTERS)
        numClusters = FAT16_MAX_CLUSTERS;
    //Eventually we won't want to format it every time
    format(false);
}

//////////////////////////
// FileSystem Interface //
//////////////////////////

int FAT16::mkdir(const char path[]) {
    KVector<char*> pathList = getPathList(path, strlen(path));
    int cluster = followPath(path, pathList.size() - 1);
    if (cluster == -1) {
        return -1;
    }
    makeDirInCluster(pathList.get(pathList.size() - 1), cluster);
    return 0;
}

int FAT16::ls(const char path[]) {
    KVector<char*> pathList = getPathList(path, strlen(path));
    int cluster = followPath(path, pathList.size());
    if (cluster == -1)
        return -1;
    listCluster(cluster);
    return 0;
}



////////////////////////
// Core Functionality //
////////////////////////

int FAT16::followPath(const char path[], int pathLength) {
    KVector<char*> pathList = getPathList(path, strlen(path));
    int currCluster = rootCluster;
        for (int i = 0; i < pathLength; i++) {
            FAT16_DirEnt dirEnt = getDirEntInCluster(pathList.get(i), currCluster);
            if (isNullDirEnt(dirEnt)) {
                kprint("No such file or directory ");
                kprint(path);
                kprint("\n");
                return -1;
            }
            currCluster = dirEnt.startingCluster;
        }
    return currCluster;
}



int FAT16::mkfile(const char path[], bool isDir) {

}

FAT16_DirEnt FAT16::makeBasicDirEnt(const char fileName[], const char extension[]){
    int nameLen = strlen(fileName);
    int extLen = strlen(extension);
    FAT16_DirEnt dir;
    memory_set(&dir, '\0', sizeof(FAT16_DirEnt));
    if (nameLen > FAT16_MAX_NAME_LEN) {
        kprint("FAT16:Name can only be 8 chars\n");
        return dir;
    }

    if (extLen > 3) {
        kprint("FAT16:Extension can only be 3 chars\n");
        return dir;
    }

    for (int i = 0; i < nameLen; i++)
        dir.fileName[i] = fileName[i];
    
    for (int i = 0; i < nameLen; i++)
        dir.fileExt[i] = extension[i];
}

int FAT16::makeDirInCluster(const char fileName[], int dirCluster) {
    FAT16_DirEnt dir = makeBasicDirEnt(fileName, "");

    //check if we can find a dirent with that filename in the cluster
    if (fileExistsInCluster(fileName, dirCluster)) {
        return -1;
    }

    setDir(dir);
    dir.startingCluster = popFreeCluster();
    makeDotAndDoubleDot(dirCluster, dir.startingCluster);
    writeDirEntToSector(dir, dirCluster);
    setFATEntry(FAT16_DIR_CLUSTER, dirCluster);
    setFATEntry((FATEntry)dir.startingCluster, FAT16_END_OF_FILE);
    return 0;
}

void FAT16::listCluster(int dirCluster) {
    for (int sector = 0; sector < SECTORS_PER_CLUSTER; sector++) {
        char FATSector[FAT16_SECTOR_SIZE];
        readSector(dirCluster, sector, FATSector);
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
    u32 sectorNum = (index / numEntriesPerSector) + startFATSector;
    u32 offset = (index % numEntriesPerSector)*4;
    char FATSector[FAT16_SECTOR_SIZE];
    fileDevice->readSector(sectorNum, FATSector, FAT16_SECTOR_SIZE);
    setFourBytes(entry, FATSector, offset);
    fileDevice->writeSector(sectorNum, FATSector, FAT16_SECTOR_SIZE);
}

int FAT16::getFATSector(u32 FATSectorNum, char FATSector[]) {
    u32 trueSectorNum = startFATSector + FATSectorNum;
    return fileDevice->readSector(trueSectorNum, FATSector, FAT16_SECTOR_SIZE);
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






///////////////////////
// Device Formatting //
///////////////////////

void FAT16::format(bool eraseData) {
    char zeroSector[FAT16_SECTOR_SIZE];
    memory_set(zeroSector, '\0', FAT16_SECTOR_SIZE);
    int i;
    char buff[10];

    if (eraseData) {
        kprint("Formatting drive...\n");
        for (i = 0; i < numClusters * SECTORS_PER_CLUSTER; i++) {
            int status = fileDevice->writeSector(i, zeroSector, FAT16_SECTOR_SIZE);
            if (status == -1) {
                kprint("FAT16.cpp:Failed to write sector\n");
            }
        }
    }
    
    writeBPB();
    writeFAT();
    u32 nextCluster = popFreeCluster();
    createRootDir();
}


void FAT16::writeBPB() {
    memory_set(BPB.reserved, '\0', FAT16_SECTOR_SIZE);
    fileDevice->writeSector(FAT16_BPB_SECTOR, (char*)&BPB, FAT16_SECTOR_SIZE);
}

void FAT16::writeFAT() {
    kprint("Writing FAT Table...\n");
    setFATEntry(FAT16_RESERVED_CLUSTER, 0);
    for (int i = 0; i < numFATClusters; i++) {
        setFATEntry(FAT16_RESERVED_CLUSTER, i + 1);
    }   
}

void FAT16::createRootDir() {
    makeDotAndDoubleDot(rootCluster, rootCluster);
}





//////////////////////
// Helper Functions //
//////////////////////

int FAT16::makeDotAndDoubleDot(int parentCluster, int dirCluster) {
    FAT16_DirEnt dot;
    FAT16_DirEnt doubleDot;
    memory_set(&dot, '\0', sizeof(FAT16_DirEnt));
    memory_set(&doubleDot, '\0', sizeof(FAT16_DirEnt));
    dot.fileName[0] = '.';
    doubleDot.fileName[0] = '.';
    doubleDot.fileName[1] = '.';
    if (!fileExistsInCluster(".", dirCluster)) {
        setDir(dot);
        dot.startingCluster = dirCluster;
        writeDirEntToSector(dot, dirCluster);
    }
    if (!fileExistsInCluster("..", dirCluster)) {
        doubleDot.startingCluster = parentCluster;
        setDir(doubleDot);
        writeDirEntToSector(doubleDot, dirCluster);
    }
    setFATEntry(dirCluster, FAT16_DIR_CLUSTER);
}

void FAT16::setFourBytes(u32 value, char buffer[], u32 offset) {
    buffer[offset] = (value >> 24) & 0xFF;
    buffer[offset + 1] = (value >> 16) & 0xFF;
    buffer[offset + 2] = (value >> 8) & 0xFF;
    buffer[offset + 3] = value & 0xFF;
}

FAT16_DirEnt FAT16::getDirEntFromSectorBuff(const char FATSector[], u32 index){
    FAT16_DirEnt dirEnt;
    u32 byteOffset = index * sizeof(FAT16_DirEnt);
    if (byteOffset > FAT16_SECTOR_SIZE) {
        kprint("FAT16:Cannot read index from sector.  Too large\n");
        return dirEnt;
    }
    memory_copy(FATSector + byteOffset, (char*)&dirEnt, sizeof(FAT16_DirEnt));
    return dirEnt;
}

u32 FAT16::getSectorOffsetForDirEnt(const char FATSector[]) {
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

u32 FAT16::popFreeCluster() {
    if (freedClusters[0] == '\0') {
        nextAvailableCluster++;
        return nextAvailableCluster - 1;
    }
    else {
        int i = 0;
        while (freedClusters[i] != '\0' && i < NUM_FREED_REMEMBERED)
            i++;
        u32 cluster = freedClusters[i - 1];
        return cluster;
    }
}

bool FAT16::fileExistsInCluster(const char fileName[], u32 clusterNum) {
    return !isNullDirEnt(getDirEntInCluster(fileName, clusterNum));
}



FAT16_DirEnt FAT16::getDirEntInCluster(const char dirName[], int parentCluster){
    for (int sector = 0; sector < SECTORS_PER_CLUSTER; sector++) {
        char FATSector[FAT16_SECTOR_SIZE];
        readSector(parentCluster, sector, FATSector);
        for (int i = 0; i < FAT16_SECTOR_SIZE; i += sizeof(FAT16_DirEnt)) {
            FAT16_DirEnt entry;
            memory_copy(FATSector + i, (char *)&entry, sizeof(FAT16_DirEnt));
            if (strCmp(entry.fileName, dirName) == 0) {
                return entry;
            }
        }
    }
    return nullDirEnt();
}

KVector<char*> FAT16::getPathList(const char path[], int pathLength) {
    int index = 1; //pass the first slash in the root directory
    KVector<char*> vector;
    while (index < pathLength) {
        char *filename = (char*)KMM.kmalloc(FAT16_MAX_NAME_LEN + 1);
        memory_set(filename, '\0', FAT16_MAX_NAME_LEN + 1);
        int charNum = 0;
        while (charNum < FAT16_MAX_NAME_LEN && index + charNum < pathLength) {
            if (path[index + charNum] == FAT16_PATH_DELIMITER) {
                filename[charNum + 1] = '\0';
                break;
            }
            else {
                filename[charNum] = path[index + charNum];
                charNum++;
            }
        }
        vector.push(filename);
        index += charNum + 1; //pass the next delimiter
    }
    return vector; 
}




/////////////////////////
// DirEnt Manipulation //
/////////////////////////
FAT16_DirEnt FAT16::nullDirEnt() {
    FAT16_DirEnt dirEnt;
    memory_set((void*)&dirEnt, '\0', sizeof(FAT16_DirEnt));
    return dirEnt;
}

bool FAT16::isNullDirEnt(FAT16_DirEnt dirEnt) {
    if (dirEnt.fileName[0] != '\0')
        return false;
    if (dirEnt.fileExt[0] != '\0')
        return false;
    return true;
}

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