#include "FS/FileSystem.h"
#include "Util/String.h"

FileSystem::FileSystem(FileDevice &theFileDevice) {
    fileDevice = &theFileDevice;
    char* buff;
    char string[] = "A string to use for today\n";
    fileDevice->writeSector(0, string, strlen(string));
    fileDevice->readSector(0, buff, strlen(string));
}