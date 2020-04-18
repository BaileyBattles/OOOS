#ifndef __FAT16_H__
#define __FAT16_H__

//TODO:
//Make freedClusters a vector (when I make the vector class)

#include "Drivers/FileDevice.h"
#include "FS/FileSystem.h"

#define FAT16_BPB_SECTOR 0
#define FAT16_SECTOR_SIZE 512
#define SECTORS_PER_CLUSTER 6
#define NUMBER_FAT_COPIES 2
#define ROOT_ENTRIES_COUNT 512

#define NUM_FREED_REMEMBERED 250 //this is to limit ammount of memory usage

#define FAT16_NUM_CLUSTERS_RESERVED 2
#define FAT16_MAX_CLUSTERS 65536
#define FAT16_AVAILABLE_CLUSTER 0x0
#define FAT16_RESERVED_CLUSTER 0xFFF0
#define FAT16_DIR_CLUSTER 0xFFF1
#define FAT16_BAD_CLUSTER 0xFFF7
#define FAT16_END_OF_FILE 0xFFF8

#define FATENTRY_PER_SECTOR 16

#define USED_BEFORE_BUT_NOW_FREE 0x05 //To work with Japanese chars

#define BPB_NUM_RESERVED 512

typedef struct {
    char fileName[8];
    char fileExt[3];
    u8 attribute;
    u8 reserved[10];
    u16 time;
    u16 date;
    u16 startingCluster;
    u32 fileSize;
} FAT16_DirEnt;

/*
Definitions:
    Filename: The last 8 bits of a path 
        Example: For /directory1/file the filename is file
    Path: The whole path
        Example: For /directory1/file the path is /directory1/file
    DirCluster:
        The Cluster which contains Directory Entries within that directory

*/


class FAT16 : public FileSystem {
public:
    FAT16(FileDevice &fileDevice);
private:
    //////////////////
    // Data Members //
    //////////////////

    int numEntriesPerSector;
    int numClusters;
    int numFATClusters;
    int startFATSector;
    int dirEntsPerSector;

    int nextAvailableCluster;
    int freedClusters[NUM_FREED_REMEMBERED];

    u32 rootCluster;

    typedef struct {
        char reserved[BPB_NUM_RESERVED];
    } FAT16BPB;

    FAT16BPB BPB;
    typedef u32 FATEntry;


    //////////////////////
    // Format the Drive //
    //////////////////////

    void format(bool eraseData);
    void writeBPB();
    void writeFAT();
    void createRootDir();
 
    ////////////////////////
    // Core Functionality //
    ////////////////////////

    //Make a dirent with filename that points to startCluster and
    //is written to homeCluster
    //Return -1 on failure
    int makeDir(char fileName[], int nameLen, int dirCluster);
    int makeFile(char fileName[], int nameLen, int startCluster);
    void ls(char path[]);
    void listCluster(int dirCluster, char path[]);

    //////////////////////
    // Device Interface //
    //////////////////////

    int readSector(u32 clusterNum, u32 sectorOffset, char FATSector[]);
    int writeSector(u32 clusterNum, u32 sectorOffset, char FATSector[]);
    void setFATEntry(FATEntry entry, u32 index);
    //Here FATSectorNum = 0 is the first sector in the FAT
    int getFATSector(u32 FATSectorNum, char FATSector[]);
    //Given a directory entry, write it to disk
    void writeDirEntToSector(FAT16_DirEnt dirEnt, u32 sectorNum);

    //////////////////////
    // Helper Functions //
    //////////////////////

    //Determine if there are 32 continuous free bytes in a sector
    //Returns -1 if there is no space
    //Returns 0 if it should be places at FATSector + 0
    //Returns 1 if it should be places at FATSector + 32
    //Returns 2 if it should be placed at FATSector + 32*2 etc..
    u32 getSectorOffsetForDirEnt(char FATSector[]);
    bool fileExistsInCluster(char fileName[], u32 clusterNum);
    void setFourBytes(u32 value, char buffer[], u32 offset);
    FAT16_DirEnt getDirEntFromSectorBuff(char FATSector[], u32 index);
    u32 getDirCluster(char path[]);

    //Search a cluster and remove it from the free list in memory 
    u32 popFreeCluster();

    int makeDotAndDoubleDot(int parentCluster, int dirCluster);
    ///////////////////////
    // DirEnt Operations //
    ///////////////////////

    bool isArchive(FAT16_DirEnt dirEnt);
    bool isDir(FAT16_DirEnt dirEnt);
    bool isVolumeID(FAT16_DirEnt dirEnt);
    bool system(FAT16_DirEnt dirEnt);
    bool isHidden(FAT16_DirEnt dirEnt);
    bool isReadOnly(FAT16_DirEnt dirEnt);

    void setArchive(FAT16_DirEnt &dirEnt);
    void setDir(FAT16_DirEnt &dirEnt);
    void setVolumeID(FAT16_DirEnt &dirEnt);
    void setSystem(FAT16_DirEnt &dirEnt);
    void setHidden(FAT16_DirEnt &dirEnt);
    void setReadOnly(FAT16_DirEnt &dirEnt);
};

#endif