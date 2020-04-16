#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "CPU/InterruptManager.h"

class Device {
public:
    virtual void initialize();
    virtual void handleInterrupt(registers_t r);
};
#endif