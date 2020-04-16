#include "CPU/InterruptManager.h"
#include "Drivers/IDE.h"
#include "Drivers/Screen.h"
#include "Sys/Io.h"
#include "Util/String.h"

//Class that implements the IDE/PATA Standard

IDE::IDE() {
    basePort = 0x1F0;
    errorPort = basePort + 1;
    sectorCountPort = basePort + 2;
    lbaLowPort = basePort + 3;
    lbaMidPort = basePort + 4;
    lbaHiPort = basePort + 5;
    devicePort = basePort + 6;
    commandPort = basePort + 7;
    controlPort = basePort + 0x206;
}

void IDE::ideWait() {
    for (u32 i = 0; i < 4; i++) {
      inb(controlPort);
    }
    while(inb(commandPort) & 0x80)
      ;
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
    outb(controlPort, 0);
    u8 status = inb(commandPort);
    if (status == 0xFF)
      kprint("No IDE Drive");
    outb(devicePort, 0);
    outb(sectorCountPort, 0);
    outb(lbaLowPort, 0);
    outb(lbaMidPort, 0);
    outb(lbaHiPort, 0);
    outb(commandPort, 0xEC);

    status = inb(commandPort);
    if (status == 0) {
      kprint("Problem");
    }

    while (status == 0x80)
      status = inb(commandPort);
    
    while (!(status & 0x8))
      status = inb(commandPort);
      
    setIDInfo();

    numSectors = idInfo.numCylinders * idInfo.numHeads * idInfo.sectorsPerTrack;
    
}

void IDE::initialize() {    
    identify();
    char buf[6];
    writeSector(15);
    readSector(15);
}

void IDE::setIDERegisters(u32 sectorNum, u32 numSectors) {
    u8 lbaByte = (sectorNum & 0xF000000) >> 24;
    outb(controlPort, 0);
    outb(sectorCountPort, numSectors);
    outb(lbaLowPort, (sectorNum & 0xFF));
    outb(lbaMidPort, (sectorNum & 0xFF00) >> 8);
    outb(lbaHiPort, (sectorNum & 0xFF00) >> 16);
    outb(devicePort, 0xE0 | lbaByte);
}

// Only supports 28 bit LBA
int IDE::readSector(u32 sectorNum){
    if (validSector(sectorNum) == -1)
        return -1;

    setIDERegisters(sectorNum, 1);
    outb(commandPort, 0x20);
    
    ideWait();
    kprint("Reading IDE");

    for (int i = 0; i < 512; i += 2) {
        char text[3];
        u16 data = inw(basePort);
        text[0] = data & 0xFF;
        text[1] = (data >> 8) & 0xFF;
        text[2] = '\0';
        char buf[10];
        kprint(text);
    }
    ideWait();

    //Make sure you read all the data!!!!! 

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

int IDE::writeSector(u32 sectorNum) {
    if (validSector(sectorNum) == -1)
        return -1;

    setIDERegisters(sectorNum, 1);
    outb(commandPort, 0x30);

    ideWait();
    kprint("Writing to IDE");

    for(int i = 0; i < 512; i+= 2){
        outl(basePort, 'c');
    }
    ideWait();
    return 0;
}

void IDE::handleInterrupt(registers_t r){}