#include "drivers/screen.h"
#include "cpu/interruptmanager.h"

void kernelLoop() {
    while (true)
    {}
}


extern "C" void main() {
    InterruptManager interruptManager;
    /* Test the interrupts */
    __asm__ __volatile__("int $2");
    __asm__ __volatile__("int $3");
    kernelLoop();
}

