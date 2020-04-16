#include "FS/FileSystem.h"
#include "Util/String.h"

FileSystem::FileSystem(FileDevice &theFileDevice) {
    fileDevice = &theFileDevice;
}