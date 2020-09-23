#include "CPU/InterruptManager.h"
#include "Drivers/IDE.h"
#include "Drivers/Screen.h"
#include "Sys/Io.h"
#include "Util/String.h"

//Class that implements the IDE/PATA Standard

IDE::IDE(u32 port) {
    basePort = port;
    errorPort = basePort + 1;
    sectorCountPort = basePort + 2;
    lbaLowPort = basePort + 3;
    lbaMidPort = basePort + 4;
    lbaHiPort = basePort + 5;
    devicePort = basePort + 6;
    statusPort = basePort + 7;
    altStatusPort = basePort + 0x206;
}

void IDE::waitBusy() {
    while (inb(altStatusPort) & IDE_BUSY)
        ;
}

void IDE::ideWait() {
    for (u32 i = 0; i < 4; i++) {
      inb(altStatusPort);
    }
    u8 status = inb(statusPort);
    while(status & IDE_BUSY)
      status = inb(statusPort);
}

void IDE::setIDInfo() {
    for (int i = 0; i < 256; i++) {
        u16 data = inw(basePort);
        // char *text = "  \0";
        // text[0] = (data >> 8) & 0xFF;
        // text[1] = data & 0xFF;
        // char buf[8];
        // kprint(int_to_ascii(data, buf));
        // kprint(" ");
        info[i] = data;
    }

  //See MSFT Doc linked in ide.h
    if (info[0] & (1 << 15)) {
        kprint("THIS IS AN ATAPI DEVICE... NOT SUPPORTED\n");
        return;
    }
    idInfo.numCylinders = info[1];
    idInfo.numHeads = info[3];
    idInfo.sectorsPerTrack = info[6];
}

void IDE::identify() {
    outb(basePort, 0xA0);
    outb(altStatusPort, 0);
    u8 status = inb(statusPort);
    if (status == 0xFF)
      kprint("No IDE Drive");
    outb(devicePort, 0);
    outb(sectorCountPort, 0);
    outb(lbaLowPort, 0);
    outb(lbaMidPort, 0);
    outb(lbaHiPort, 0);
    outb(statusPort, 0xEC);

    status = inb(statusPort);
    if (status == 0) {
      kprint("Problem");
    }

    while (status & IDE_BUSY)
      status = inb(statusPort);
    
    while (!(status & 0x8))
      status = inb(statusPort);
      
    setIDInfo();

    numSectors = idInfo.numCylinders * idInfo.numHeads * idInfo.sectorsPerTrack;
    
}

void IDE::initialize() {    
    identify();
    char buf[512];
    char string[] = "This is my string that I am using\n";
    //writeSector(16, string, 35);
    //readSector(12, buf, 36);
}

void IDE::setIDERegisters(u32 sectorNum, u32 numSectors) {
    u8 lbaByte = (sectorNum & 0xF000000) >> 24;
    outb(altStatusPort, 0);
    outb(sectorCountPort, numSectors);
    outb(lbaLowPort, (sectorNum & 0xFF));
    outb(lbaMidPort, (sectorNum & 0xFF00) >> 8);
    outb(lbaHiPort, (sectorNum & 0xFF00) >> 16);
    outb(devicePort, 0xE0 | lbaByte);
}

// Only supports 28 bit LBA
// Size better be the
int IDE::readSector(u32 sectorNum, char* buffer, u32 size){
    if (validSector(sectorNum) == -1)
        return -1;

    ideWait();
    setIDERegisters(sectorNum, 1);
    outb(statusPort, 0x20);
    u16 dataBuf[256];
    char buff[512];
    int index = 0;
    while (index < size) {
        waitBusy();
        u16 data= inw(basePort);
        buffer[index] = data & 0xFF;
        if (index + 1 < size)
            buffer[index + 1] = ((data & 0xFF00) >> 8);
        index += 2;
    }    
    //Make sure you read all the data!!!!! 
    for (; index < 512; index += 2)
        inw(basePort);
    ideWait();

    return 0;   
}



int IDE::validSector(u32 sectorNum) {
    if (sectorNum > 0xFFFFFFF) {
        kprint("IDE only supports 28 bit LBA, Sector Num is too big\n");
        return -1;
    }
    if (sectorNum > numSectors) {
        kprint("Invalid sector number ");
        char errorBuffer[10];
        kprint(int_to_ascii(sectorNum, errorBuffer));
        kprint(". There are only ");
        char errorBuffer2[10];
        kprint(int_to_ascii(numSectors, errorBuffer));
        kprint(" sectors.\n");
        return -1;
    }
    return 0;
}

int IDE::writeSector(u32 sectorNum, char* buffer, u32 size) {
    if (validSector(sectorNum) == -1)
        return -1;

    setIDERegisters(sectorNum, 1);
    outb(statusPort, 0x30);

    ideWait();
    int index;
    for(index = 0; index < size; index+= 2){
        waitBusy();
        u16 data = (buffer[index] & 0xFF);
        if (index + 1 < size)
            data |= (buffer[index + 1] << 8) & 0xFF00;
        outw(basePort, data);
    }

    for (; index < 512; index += 2)
        outw(basePort, 0);

    ideWait();

    flushCache();
    return 0;
}

int IDE::deviceSize() {
    return numSectors * IDE_SECTOR_SIZE;
}

int IDE::sectorSize() {
    return IDE_SECTOR_SIZE;
}

void IDE::flushCache() {
    outb(statusPort, 0xE7);
    waitBusy();
}

void IDE::handleInterrupt(registers_t r){}