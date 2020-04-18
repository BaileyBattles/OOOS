#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "CPU/InterruptManager.h"

class Device {
public:
    virtual void initialize()=0;
    virtual void handleInterrupt(registers_t r)=0;
};
#endif