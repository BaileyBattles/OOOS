#include "cpu/interrupt_manager.h"
#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "memory/kMemoryManger.h"
#include "util/string.h"

extern unsigned long ebss;

void kernelLoop() {
    while (true)
    {}
}


extern "C" void main(void *stackBase, u32 addr1) {
    clear_screen();
    void *endOfBss = &ebss;
    InterruptManager interruptManager;
    Keyboard keyboard;
    keyboard.initialize();
    /* Test the interrupts */
    __asm__ __volatile__("int $2");
    __asm__ __volatile__("int $3");


    kernelLoop();
}

