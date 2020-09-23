#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "CPU/InterruptManager.h"
#include "Drivers/Device.h"
#include "Process/Process.h"
#include "Process/IPC.h"

class Keyboard : public Device {
public:
    Keyboard();
    virtual void initialize();
    virtual void handleInterrupt(registers_t r);
    void registerTerminal(Process *process);
private:
    char translateInput(u8 input);
    char accountForShift(char input);
    bool shiftDown;

    Process* terminal;
    IPCSocket socket;
};

#endif