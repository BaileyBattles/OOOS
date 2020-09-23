#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "CPU/InterruptHandlingObject.h"
#include "CPU/InterruptManager.h"

class Device : public InterruptHandlingObject {
public:
    virtual void initialize()=0;
    virtual void handleInterrupt(registers_t r)=0;
};


#endif