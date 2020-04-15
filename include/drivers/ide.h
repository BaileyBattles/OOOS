#include "drivers/device.h"

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

    void identify();
    void ideWait();
    void setLBARegisters(u32 sectorNum);
};