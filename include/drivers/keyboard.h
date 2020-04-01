#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "cpu/interrupt_manager.h"
#include "drivers/device.h"

class Keyboard : public Device {
public:
    void initialize();
    void handleInterrupt(registers_t r);
};

#endif