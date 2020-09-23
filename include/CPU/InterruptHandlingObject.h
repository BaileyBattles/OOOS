#ifndef __INTERRUPT_HANDLING_OBJECT_H__
#define __INTERRUPT_HANDLING_OBJECT_H__

#include "CPU/InterruptManager.h"

class InterruptHandlingObject {
public:
    virtual void initialize()=0;
    virtual void handleInterrupt(registers_t r)=0;
};
#endif