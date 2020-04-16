#include "FS/FileSystem.h"
#include "Util/String.h"

FileSystem::FileSystem(FileDevice &theFileDevice) {
    fileDevice = &theFileDevice;
    char* buff;
    char string[] = "Different abababababab string this time\n";
    fileDevice->writeSector(20, string, strlen(string));
    fileDevice->readSector(20, buff, strlen(string));
}