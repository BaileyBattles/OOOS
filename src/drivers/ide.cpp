#include "cpu/interrupt_manager.h"
#include "drivers/ide.h"
#include "drivers/screen.h"
#include "sys/io.h"
#include "util/string.h"

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

void IDE::identify() {
    outb(basePort, 0xA0);
    outb(controlPort, 0);
    u8 status = inb(commandPort);
    if (status == 0xFF)
      kprint("No IDE Drive");
    outb(devicePort, 0);
    outb(sectorCountPort, 0);
    setLBARegisters(0);
    outb(commandPort, 0xEC);

    status = inb(commandPort);
    if (status == 0) {
      kprint("Problem");
    }

    while (status == 0x80)
      status = inb(commandPort);
    
    while (!(status & 0x8))
      status = inb(commandPort);
      
    char buf[5];
    kprint(int_to_ascii(status, buf));

    for (int i = 0; i < 256; i++) {
        u16 data = inw(basePort);
        char *text = "  \0";
        text[0] = (data >> 8) & 0xFF;
        text[1] = data & 0xFF;
        kprint(text);
    }
}

void IDE::initialize() {    
    identify();
    kprint("\n\n\n");
    readSector(5);
    kprint("\n\n\n");
    writeSector(9);
    readSector(9);

}

// Only supports 28 bit LBA
void IDE::readSector(u32 sectorNum){
    if (sectorNum > 0xFFFFFFF) {
        kprint("IDE only supports 28 bit LBA, Sector Num is too big");
    }
    u8 lbaByte = (sectorNum & 0xF000000) >> 24;
    outb(controlPort, 0);
    outb(sectorCountPort, 1);
    setLBARegisters(sectorNum);

    outb(devicePort, 0xE0 | lbaByte);
    outb(commandPort, 0x20);

    u8 status = inb(commandPort);

    
    if(status & 0x01)
    {
        kprint("ERROR");
        return;
    }
    
    ideWait();
    kprint("Reading IDE");

    for (int i = 0; i < 20; i++)
        status = inb(commandPort);

    for (int i = 0; i < 512; i += 2) {
        char text[3];
        u16 data = inw(basePort);
        text[0] = data & 0xFF;
        text[1] = (data >> 8) & 0xFF;
        text[2] = '\0';
        char buf[10];
        kprint(int_to_ascii(data, buf));
        kprint(" ");
    }
    ideWait();

    //Make sure you read all the data!!!!!    
}

void IDE::writeSector(u32 sectorNum) {
    if (sectorNum > 0xFFFFFFF) {
            kprint("IDE only supports 28 bit LBA, Sector Num is too big");
        }
    u8 lbaByte = (sectorNum & 0xF000000) >> 24;
    outb(devicePort, 0xE0 | lbaByte);
    outb(errorPort, 0);
    outb(sectorCountPort, 1);
    setLBARegisters(sectorNum);
    outb(commandPort, 0x30);

    ideWait();
    kprint("Writing to IDE");

    for(int i = 0; i < 512; i+= 2){
        outl(basePort, 'c');
    }
    ideWait();

}

void IDE::setLBARegisters(u32 sectorNum) {
    outb(lbaLowPort, (sectorNum & 0xFF));
    outb(lbaMidPort, (sectorNum & 0xFF00) >> 8);
    outb(lbaHiPort, (sectorNum & 0xFF00) >> 16);
}

void IDE::handleInterrupt(registers_t r){}