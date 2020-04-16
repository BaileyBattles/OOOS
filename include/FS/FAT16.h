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

class FAT16 : public FileSystem {
public:
    FAT16(FileDevice &fileDevice);
private:
    int numClusters;
    typedef struct {

        char reserved[BPB_NUM_RESERVED];
    } FAT16BPB;
    FAT16BPB BPB;

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

    typedef u32 FATEntry;

    void format();
    void writeBPB();
    void writeFATs();

    //File Attributes
    bool isArchive(FAT16_DirEnt dirEnt);
    bool isDir(FAT16_DirEnt dirEnt);
    bool isVolumeID(FAT16_DirEnt dirEnt);
    bool system(FAT16_DirEnt dirEnt);
    bool isHidden(FAT16_DirEnt dirEnt);
    bool isReadOnly(FAT16_DirEnt dirEnt);
};

#endif