#include "Drivers/Pci.h"
#include "Drivers/Screen.h"

#include "Sys/Io.h"
#include "Util/String.h"

void PCI::writeToAddressPort(u32 bus, u32 device, u32 function, u32 offset) {
    u32 output = (1 << 31)       |
                 (bus << 16)     |
                 (device << 11)  |
                 (function << 8) |
                 offset;
    outl(PCI_ADDR_PORT, output);
}

u32 PCI::readFromDataPort(){
    return inl(PCI_DATA_PORT);
}

bool PCI::deviceExists(u32 bus, u32 device, u32 function){
    writeToAddressPort(bus, device, function, 8);
    u32 val = readFromDataPort();
    if (val == 0xFFFF)
        return false;
    return true;
}

u32 PCI::read(u16 bus, u16 device, u16 func, u32 offset)
{
    u32 id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((func & 0x07) << 8)
        | (offset & 0xFC);
    outl(0xCF8, id);
    u32 result = inl(0xCFC);
    return result >> (8* (offset % 4));
}

u16 PCI::classID(u16 bus, u16 device, u16 func) {
    return (u16)read(bus, device, func, 0x0b) & 0xFF;
}

u16 PCI::subclassID(u16 bus, u16 device, u16 func) {
    return (u16)read(bus, device, func, 0x0a) & 0xFF;
}

void PCI::scan() {
    for (int bus = 0; bus < 8; bus++) {
        for (int device = 0; device < 32; device++){
            for (int func = 0; func < 8; func++){
                char classIDBuf[5];
                char subclassIDBuf[5];
                u16 classIdNum = classID(bus, device, func);
                u16 subclassIdNum = subclassID(bus, device, func);
                if (classIdNum != 255) {
                    kprint(int_to_ascii(classIdNum, classIDBuf));
                    kprint(" ");
                    kprint(int_to_ascii(subclassIdNum, classIDBuf));
                    kprint("\n");
                }
            }
        }
    }
}
