#include "cpu/interrupt_manager.h"
#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "util/string.h"

void kernelLoop() {
    while (true)
    {}
}


extern "C" void main(void *stackBase) {
    clear_screen();
    extern int stack_ptr;
    char buffer[10];
    kprint(int_to_ascii((int)stackBase, buffer));
    InterruptManager interruptManager;
    Keyboard keyboard;
    keyboard.initialize();
    registerHandler(&keyboard, 33);
    /* Test the interrupts */
    __asm__ __volatile__("int $2");
    __asm__ __volatile__("int $3");
    kernelLoop();
}

