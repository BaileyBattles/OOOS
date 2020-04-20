#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Drivers/FileDevice.h"

class FileSystem {
public:
    FileSystem(FileDevice &theFileDevice);
    virtual int readNBytes(const char path[], char buffer[], int nBytes) = 0;
    virtual int writeNBytes(const char path[], char buffer[], int nBytes) = 0;
    virtual int mkdir(const char path[]) = 0;
    virtual int mkfile(const char path[]) = 0;
    virtual int ls(const char path[]) = 0;
    virtual int sectorSize() = 0;
protected:
    FileDevice *fileDevice;
};

#endif