#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "cpu/interrupt_manager.h"
#include "drivers/device.h"

class Keyboard : public Device {
public:
    Keyboard();
    void initialize();
    void handleInterrupt(registers_t r);
private:
    char translateInput(u8 input);
    char accountForShift(char input);
    bool shiftDown;
};

#endif