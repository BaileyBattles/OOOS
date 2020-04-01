#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "cpu/interrupt_manager.h"

class Device {
public:
    virtual void initialize();
    virtual void handleInterrupt(registers_t r);
};
#endif