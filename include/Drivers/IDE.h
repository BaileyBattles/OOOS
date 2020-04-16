#include "Drivers/FileDevice.h"

#define IDE_SECTOR_SIZE 512

typedef struct {
    u16 numCylinders;
    u16 numHeads;
    u16 sectorsPerTrack;
} IdInfo;

class IDE : public FileDevice {
public:
    IDE(u32 port);
    void initialize();
    void handleInterrupt(registers_t r);
    int readSector(u32 sectorNum, char* buffer, u32 size);
    int writeSector(u32 sectorNum, char* buffer, u32 size);

    int deviceSize();
private:
    u16 basePort;
    u16 errorPort;
    u16 sectorCountPort;
    u16 lbaLowPort;
    u16 lbaMidPort;
    u16 lbaHiPort;
    u16 devicePort;
    u16 commandPort;
    u16 controlPort;
    
    u32 numSectors; //can be u32 because using 28 bit LBA

    //ATA Info in FYSOS Ch 7 or 
    //https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data
    u16 info[256];
    IdInfo idInfo;
    

    void identify();
    void ideWait();
    void setIDERegisters(u32 sectorNum, u32 numSectors);
    void setIDInfo();
    int validSector(u32 sectorNum);
};