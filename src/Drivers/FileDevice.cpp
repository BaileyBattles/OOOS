#include "Drivers/FileDevice.h"

void FileDevice::initialize(){}
void FileDevice::handleInterrupt(registers_t r){}
int FileDevice::readSector(u32 sectorNum, char* buffer, u32 size){}
int FileDevice::writeSector(u32 sectorNum, char* buffer, u32 size){}
int FileDevice::deviceSize(){}