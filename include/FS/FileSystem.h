#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Drivers/FileDevice.h"

class FileSystem {
public:
    FileSystem(FileDevice &theFileDevice);
private:
    FileDevice *fileDevice;
};

#endif