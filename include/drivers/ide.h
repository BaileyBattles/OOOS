#include "drivers/device.h"

typedef struct {
    u16 numCylinder;
    u16 numHeads;
    u16 sectorsPerTrack;
} IdInfo;

class IDE : Device {
public:
    IDE();
    void initialize();
    void handleInterrupt(registers_t r);
    void readSector(u32 sectorNum);
    void writeSector(u32 sectorNum);
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

    //ATA Info in FYSOS Ch 7 or 
    //https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data
    u16 info[256];
    IdInfo idInfo;
    

    void identify();
    void ideWait();
    void setIDERegisters(u32 sectorNum, u32 numSectors);
    void setIDInfo();
};