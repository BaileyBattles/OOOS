#ifndef __PCI_H__
#define __PCI_H__

#include "Kernel/Types.h"

#define PCI_ADDR_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

class PCI{
public:
    static PCI& the()
    {
        static PCI instance;
        return instance;
    }    
    bool deviceExists(u32 bus, u32 device, u32 function);
    u16 classID(u16 bus, u16 device, u16 func);
    u16 subclassID(u16 bus, u16 device, u16 func);
    void scan();




private:
    void writeToAddressPort(u32 bus, u32 device, u32 function, u32 offset);
    u32 readFromDataPort();
    u32 read(u16 bus, u16 device, u16 func, u32 offset);
    void checkIDE(int bus, int device, int func);

};

#endif