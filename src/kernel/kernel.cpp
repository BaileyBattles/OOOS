#include "cpu/interrupt_manager.h"
#include "drivers/keyboard.h"
#include "drivers/screen.h"

void kernelLoop() {
    while (true)
    {}
}


extern "C" void main() {
    clear_screen();
    InterruptManager interruptManager;
    Keyboard keyboard;
    keyboard.initialize();
    registerHandler(&keyboard, 33);
    /* Test the interrupts */
    __asm__ __volatile__("int $2");
    __asm__ __volatile__("int $3");
    kernelLoop();
}

