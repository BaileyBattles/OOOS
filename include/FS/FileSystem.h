#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Drivers/FileDevice.h"
#include "FS/File.h"

class FileSystem {
public:
    FileSystem(FileDevice &theFileDevice);
    virtual File getFile(const char path[]) = 0;
    virtual int readNBytes(int startSector, int nBytes) = 0;
    virtual int mkdir(const char path[]) = 0;
    virtual int ls(const char path[]) = 0;
protected:
    FileDevice *fileDevice;
};

#endif