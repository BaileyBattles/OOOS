#ifndef __FAT16_H__
#define __FAT16_H__

#include "Drivers/FileDevice.h"
#include "FS/FileSystem.h"

#define FAT16_BPB_SECTOR 0
#define FAT16_SECTOR_SIZE 512
#define SECTORS_PER_CLUSTER 6
#define NUMBER_FAT_COPIES 2
#define ROOT_ENTRIES_COUNT 512

#define FAT16_NUM_CLUSTERS_RESERVED 2
#define FAT16_MAX_CLUSTERS 65536
#define FAT16_AVAILABLE_CLUSTER 0x0
#define FAT16_RESERVED_CLUSTER 0xFFF0
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

class FAT16 : public FileSystem {
public:
    FAT16(FileDevice &fileDevice);
private:
    int numEntriesPerSector;
    int numClusters;
    int numFATClusters;
    int startFAT;

    u32 rootCluster;

    typedef struct {
        char reserved[BPB_NUM_RESERVED];
    } FAT16BPB;
    FAT16BPB BPB;



    typedef u32 FATEntry;

    void format(bool eraseData);
    void writeBPB();
    void writeFAT();
    void createRootDir();
 
    //Make a dirent with filename that points to startCluster and
    //is written to homeCluster
    FAT16_DirEnt makeDir(char fileName[], int nameLen, int startCluster, int homeCluster);
    FAT16_DirEnt makeFile(char fileName[], int nameLen, int startCluster);
    
    void ls(int homeCluster);
    //Given a directory entry, write it to disk
    void writeDirEntToSector(FAT16_DirEnt dirEnt, u32 sectorNum);


    //Determine if there are 32 continuous free bytes in a sector
    //Returns -1 if there is no space
    //Returns 0 if it should be places at FATSector + 0
    //Returns 1 if it should be places at FATSector + 32
    //Returns 2 if it should be placed at FATSector + 32*2 etc..
    u32 getSectorOffsetForDirEnt(char FATSector[]);

    void setFATEntry(FATEntry entry, u32 index);
    void setFourBytes(u32 value, char buffer[], u32 offset);
    u32 dirEntToU32(FAT16_DirEnt dirEnt);
    //File Attributes
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