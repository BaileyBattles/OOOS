#ifndef __FILEDEVICE_H__
#define __FILEDEVICE_H__

#include "Drivers/Device.h"
#include "Kernel/Types.h"

class FileDevice : public Device {
public:
    virtual void initialize();
    virtual void handleInterrupt(registers_t r);
    virtual int readSector(u32 sectorNum, char* buffer, u32 size);
    virtual int writeSector(u32 sectorNum, char* buffer, u32 size);

    virtual int deviceSize();
};

#endif