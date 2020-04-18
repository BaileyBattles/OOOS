#ifndef __FILEDEVICE_H__
#define __FILEDEVICE_H__

#include "Drivers/Device.h"
#include "Kernel/Types.h"

class FileDevice : public Device {
public:
    virtual void initialize()=0;
    virtual void handleInterrupt(registers_t r)=0;
    virtual int readSector(u32 sectorNum, char* buffer, u32 size)=0;
    virtual int writeSector(u32 sectorNum, char* buffer, u32 size)=0;

    virtual int deviceSize()=0;
    virtual int sectorSize()=0;
};

#endif