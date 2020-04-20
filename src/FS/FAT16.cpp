#include "Drivers/Screen.h"
#include "FS/FAT16.h"
#include "Memory/KMemoryManager.h"
#include "Util/Memcpy.h"
#include "Util/String.h"

FAT16::FAT16(FileDevice &theFileDevice) :
    FileSystem(theFileDevice) 
{
    numFATEntriesPerSector = FAT16_SECTOR_SIZE / (sizeof(FATEntry));
    u32 numEntriesPerCluster = numFATEntriesPerSector * SECTORS_PER_CLUSTER;
    numClusters = fileDevice->deviceSize() / (SECTORS_PER_CLUSTER * FAT16_SECTOR_SIZE);
    if (numClusters > FAT16_MAX_CLUSTERS)
        numClusters = FAT16_MAX_CLUSTERS;
    numFATClusters = numClusters / numEntriesPerCluster;

    rootCluster = numFATClusters + 1;
    nextAvailableCluster = rootCluster + 1;
    memory_set(freedClusters, '\0', NUM_FREED_REMEMBERED);
    dirEntsPerSector = FAT16_SECTOR_SIZE / sizeof(FAT16_DirEnt);

    startFATSector = SECTORS_PER_CLUSTER * 1; //start in the second cluster

    //Eventually we won't want to format it every time
    format(false);
}

//////////////////////////
// FileSystem Interface //
//////////////////////////


File *FAT16::getFile(const char path[]){
    FAT16_DirEnt dirEnt = followPath(path, 0);
    if (isNullDirEnt(dirEnt)) {
        return nullptr;
    }

    FileInfo fileInfo;
    fileInfo.startSector = dirEnt.startingCluster * SECTORS_PER_CLUSTER;
    fileInfo.size = 0;
    File *filePtr = (File*)KMM.kmalloc(sizeof(File));
    File file(*this, fileInfo);
    memory_copy((char*)&file, (char*)filePtr, sizeof(File));
    return filePtr;
}

int FAT16::readNBytes(const char path[], char buffer[], int nBytes){
    FAT16_DirEnt parent = followPath(path, 1);
    FAT16_DirEnt file = followPath(path, 0);
    if (isNullDirEnt(parent) || isNullDirEnt(file)) {
        return -1;
    }
    int clusterNum = file.startingCluster;
    int clusterOffset = 0;
    KVector<FATEntry> clustersToRead = followFATEntries(clusterNum);

    int numSectors = nBytes / FAT16_SECTOR_SIZE + 1;
    int sectorsRead = 0;

    for (int i = 0; i < clustersToRead.size(); i++) {
        int clusterToRead = clustersToRead.get(i);
        for (int sectorNum = 0; sectorNum < SECTORS_PER_CLUSTER; sectorNum++) {
            u32 sector = clusterNum * SECTORS_PER_CLUSTER + sectorNum;
            u32 bytesToRead = FAT16_SECTOR_SIZE;
            if (nBytes < FAT16_SECTOR_SIZE)
                bytesToRead = nBytes;
            int status = fileDevice->readSector(sector, 
                                    buffer + sectorsRead * FAT16_SECTOR_SIZE,
                                    bytesToRead);
            if (status == -1) {
                kprint("FAT16:readNBytes: Write Sector failed\n");
                return -1;
            }
            sectorsRead++;
            nBytes -= FAT16_SECTOR_SIZE;
            if (sectorsRead == numSectors) {
                return 0;
            }
        }
    }
    kprint("FAT16:readNBytes Did not read all requested bytes");
    return 0;
}

// This also handles allocating new sectors if needed
int FAT16::writeNBytes(const char path[], char buffer[], int nBytes){
    FAT16_DirEnt parent = followPath(path, 1);
    FAT16_DirEnt file = followPath(path, 0);
    if (isNullDirEnt(parent) || isNullDirEnt(file)) {
        return -1;
    }

    FAT16_DirEnt dirEnt;
    setFilename(dirEnt, file.fileName);
    int index = getDirEntInCluster(dirEnt, parent.startingCluster);
    int numSectors = nBytes / FAT16_SECTOR_SIZE + 1;

    int clusterNum = file.startingCluster;
    int clusterOffset = 0;
    int bytesWritten = 0;
    for (int i = 0; i < numSectors; i++) {
        if (clusterOffset >= SECTORS_PER_CLUSTER) {
            FATEntry entry = getFATEntry(clusterNum);
            if (entry == 0 || entry == FAT16_END_OF_FILE) {
                int newClusterNum = popFreeCluster();
                setFATEntry(newClusterNum, clusterNum);
                clusterNum = newClusterNum;
                setFATEntry(FAT16_END_OF_FILE, clusterNum);
            }
            else {
                clusterNum = entry;
            }
            clusterOffset = 0;
        }
        int currentSector = clusterNum * SECTORS_PER_CLUSTER + clusterOffset;
        int bytesToWrite = FAT16_SECTOR_SIZE;
        if (nBytes - bytesWritten < FAT16_SECTOR_SIZE) {
            bytesToWrite = nBytes - bytesWritten;
        }
        int status = fileDevice->writeSector(currentSector,
                                            buffer, bytesToWrite);
        clusterOffset++;
        bytesWritten -= bytesToWrite;
        if (status == -1) {
            kprint("FAT16:writeNBytes: Write Sector failed\n");
            return -1;
        }
    }
    dirEnt.fileSize = nBytes;
    writeDirEntToSector(dirEnt, parent.startingCluster);
    return 0;
}



int FAT16::mkdir(const char path[]) {
    KVector<char*> pathList = getPathList(path, strlen(path));
    FAT16_DirEnt dirEnt = followPath(path, 1);
    if (isNullDirEnt(dirEnt)) {
        return -1;
    }
    int cluster = dirEnt.startingCluster;
    makeEntryInCluster(pathList.get(pathList.size() - 1), cluster, true);
    return 0;
}


int FAT16::mkfile(const char path[]) {
    KVector<char*> pathList = getPathList(path, strlen(path));
    FAT16_DirEnt dirEnt = followPath(path, 1);
    if (isNullDirEnt(dirEnt)) {
        return -1;
    }
    int cluster = dirEnt.startingCluster;
    makeEntryInCluster(pathList.get(pathList.size() - 1), cluster, false);
    return 0;
}

int FAT16::ls(const char path[]) {
    KVector<char*> pathList = getPathList(path, strlen(path));
    FAT16_DirEnt dirEnt = followPath(path, 0);
    if (isNullDirEnt(dirEnt)) {
        return -1;
    }
    int cluster = dirEnt.startingCluster;
    listCluster(cluster);
    return 0;
}



////////////////////////
// Core Functionality //
////////////////////////

FAT16_DirEnt FAT16::followPath(const char path[], int stopBefore) {
    KVector<char*> pathList = getPathList(path, strlen(path));
    int currCluster = rootCluster;
    FAT16_DirEnt currEntry = rootDirEnt();
    for (int i = 0; i < pathList.size() - stopBefore; i++) {
        FAT16_DirEnt dirEnt;
        setFilename(dirEnt, pathList.get(i));
        int dirEntIndex = getDirEntInCluster(dirEnt, currCluster);
        if (isNullDirEnt(dirEnt)) {
            kprint("No such file or directory ");
            kprint(path);
            kprint("\n");
            return nullDirEnt();
        }
        currCluster = dirEnt.startingCluster;
        currEntry = dirEnt;
    }
    return currEntry;
}

KVector<FAT16::FATEntry> FAT16::followFATEntries(int startingCluster) {
    //Example If the file's first cluster was at cluster 35 and there are
    //32 FAT entries per Sector then the FAT Entry will lie in the 
    //2nd FAT Sector with an offset of 3
    int FATSectorNum = startingCluster / numFATEntriesPerSector;
    int FATSectorOffset = startingCluster % numFATEntriesPerSector;

    KVector<FAT16::FATEntry> clusters;
    clusters.push(startingCluster);
    char FATSector[FAT16_SECTOR_SIZE];

    getFATSector(FATSectorNum, FATSector);
    FATEntry *nextClusterPtr = (FATEntry*)(FATSector + FATSectorOffset*sizeof(FATEntry));
    while (*nextClusterPtr != FAT16_END_OF_FILE) {
        clusters.push(*nextClusterPtr);

        FATSectorNum = *nextClusterPtr / numFATEntriesPerSector;
        FATSectorOffset = *nextClusterPtr % numFATEntriesPerSector;
        getFATSector(FATSectorNum, FATSector);
        nextClusterPtr = (FATEntry*)(FATSector + FATSectorOffset*sizeof(FATEntry));
    }
    return clusters;
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
    
    for (int i = 0; i < extLen; i++)
        dir.fileExt[i] = extension[i];
}

int FAT16::makeEntryInCluster(const char fileName[], int dirCluster, bool isDir) {
    FAT16_DirEnt dir = makeBasicDirEnt(fileName, "");

    //check if we can find a dirent with that filename in the cluster
    if (fileExistsInCluster(fileName, dirCluster)) {
        return -1;
    }
    dir.startingCluster = popFreeCluster();
    if (isDir){
        setDir(dir);
        setFATEntry(FAT16_DIR_CLUSTER, dir.startingCluster);
        makeDotAndDoubleDot(dirCluster, dir.startingCluster);
    }
    else {
        setFATEntry(FAT16_END_OF_FILE, dir.startingCluster);
    }
    writeDirEntToSector(dir, dirCluster);
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
    u32 FATSectorNum = (index / numFATEntriesPerSector) + startFATSector;
    u32 offset = (index % numFATEntriesPerSector)*sizeof(FATEntry);
    char FATSector[FAT16_SECTOR_SIZE];
    char buff[10];
    fileDevice->readSector(FATSectorNum, FATSector, FAT16_SECTOR_SIZE);
    //setFourBytes(entry, FATSector, offset);
    memory_copy((char*)&entry, FATSector + offset, sizeof(FATEntry));
    fileDevice->writeSector(FATSectorNum, FATSector, FAT16_SECTOR_SIZE);
}

FAT16::FATEntry FAT16::getFATEntry(u16 clusterNum) {
    u32 FATSectorNum = clusterNum / numFATEntriesPerSector;
    u32 FATSectorOffset = clusterNum % numFATEntriesPerSector;
    
    u32 trueSectorNum = startFATSector + FATSectorNum;
    FATEntry FATSector[numFATEntriesPerSector];
    fileDevice->readSector(trueSectorNum, (char*)FATSector, FAT16_SECTOR_SIZE);
    return FATSector[FATSectorOffset];
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
        offset = getSectorOffsetForDirEnt(FATSector, dirEnt);
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
    createRootDir();
    findFreeClusters();
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
    setFATEntry(FAT16_DIR_CLUSTER, rootCluster);
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
}

u32 FAT16::getSectorOffsetForDirEnt(const char FATSector[], FAT16_DirEnt dirEnt) {
    for (int i = 0; i < FAT16_SECTOR_SIZE; i += sizeof(FAT16_DirEnt)) {
        bool isSpace = true;
        FAT16_DirEnt currEntry;
        memory_copy(FATSector + i, (char*)&currEntry, sizeof(FAT16_DirEnt));
        int length = strlen(dirEnt.fileName) < FAT16_MAX_NAME_LEN ? strlen(dirEnt.fileName) : FAT16_MAX_NAME_LEN;
        if (memory_cmp(currEntry.fileName, dirEnt.fileName, length) == 0) {
            return i;
        }
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
    if (freeClusters.size() == 0) {
        return 0;
    }
    u32 value = freeClusters.get(0);
    freeClusters.remove(0);
    u16 start = value & 0xFFFF;
    u16 length = (value >> 16) & 0xFFFF;
    u16 newStart = start + 1;
    u16 newLength = length - 1;
    if (newLength > 0) {
        u32 value = newStart |= ((newLength << 16) & 0xFFFF);
        freeClusters.put(value, 0);
    }
    return start;
}

bool FAT16::fileExistsInCluster(const char fileName[], u32 clusterNum) {
    FAT16_DirEnt dirEnt;
    setFilename(dirEnt, fileName);
    return (getDirEntInCluster(dirEnt, clusterNum) != -1);
}

int FAT16::compareDirEntNames(FAT16_DirEnt dirEnt1, FAT16_DirEnt dirEnt2) {
    if (strlen(dirEnt1.fileName) != strlen(dirEnt2.fileName)) {
        return -1;
    }
    for (int i = 0; i < strlen(dirEnt1.fileName); i++) {
        if (dirEnt1.fileName[i] != dirEnt2.fileName[i])
            return -1;
    }
    return 0;
}   


int FAT16::getDirEntInCluster(FAT16_DirEnt &dirEnt, int parentCluster){
    for (int sector = 0; sector < SECTORS_PER_CLUSTER; sector++) {
        char FATSector[FAT16_SECTOR_SIZE];
        readSector(parentCluster, sector, FATSector);
        for (int i = 0; i < FAT16_SECTOR_SIZE; i += sizeof(FAT16_DirEnt)) {
            FAT16_DirEnt entry;
            memory_copy(FATSector + i, (char *)&entry, sizeof(FAT16_DirEnt));
            if (compareDirEntNames(entry, dirEnt) == 0) {
                dirEnt = entry;
                return i / sizeof(FAT16_DirEnt);
            }
        }
    }
    return -1;
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

void FAT16::findFreeClusters() {
    int lastUsed = -1;
    for (int i = 0; i < numFATClusters * SECTORS_PER_CLUSTER; i++) {
        FATEntry FATSector[numFATEntriesPerSector];
        fileDevice->readSector(i + startFATSector, (char*)FATSector, FAT16_SECTOR_SIZE);
        for (int j = 0; j < numFATEntriesPerSector; j++) {
            if (FATSector[j] != FAT16_AVAILABLE_CLUSTER) {
                int currentCluster = i * numFATEntriesPerSector + j;
                int numFree = currentCluster - lastUsed - 1;
                if (numFree > 0) {
                    u32 value = lastUsed + 1;
                    value |= ((numFree << 16) & 0xFFFF0000);
                    freeClusters.push(value);
                }
                lastUsed = currentCluster;
            }
        }
    }
    int numFree = numFATClusters * SECTORS_PER_CLUSTER * numFATEntriesPerSector - lastUsed - 1;
    if (numFree > 0) {
        u32 value = lastUsed + 1;
        value |= ((numFree << 16) & 0xFFFF0000);
        freeClusters.push(value);
    }
}




/////////////////////////
// DirEnt Manipulation //
/////////////////////////
FAT16_DirEnt FAT16::rootDirEnt() {
    FAT16_DirEnt dirEnt;
    memory_set((void*)&dirEnt, '\0', sizeof(FAT16_DirEnt));
    dirEnt.fileName[0] = '.';
    setDir(dirEnt);
    dirEnt.startingCluster = rootCluster;
    return dirEnt;
}

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

void FAT16::setFilename(FAT16_DirEnt &dirEnt, const char filename[]) {
    int length = strlen(filename) + 1 < FAT16_MAX_NAME_LEN ? strlen(filename) + 1 : FAT16_MAX_NAME_LEN;
    memory_copy(filename, dirEnt.fileName, length);
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

bool FAT16::isFile(FAT16_DirEnt dirEnt) {
    return (!isDir(dirEnt) && !isVolumeID(dirEnt) && 
            !isArchive(dirEnt) && !system(dirEnt));
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