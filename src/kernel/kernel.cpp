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

void testInterrupts() {
    //__asm__ __volatile__("int $2");
    //__asm__ __volatile__("int $3");
}

extern "C" void main(void *stackBase, u32 addr1) {
    clear_screen();
    u32 endOfBss = (u32)&ebss;
    InterruptManager interruptManager;
    Keyboard keyboard;
    keyboard.initialize();

    //Idealy don't want KMM on stack, should be in BSS I think
    KMemoryManager KMM(endOfBss);
    char *buffer = (char*)KMM.kmalloc(32);
    if (buffer == NULL){
        kprint("Failed to initialize buffer");
    }
    char *buffer2 = (char*)KMM.kmalloc(32);

    kernelLoop();
}

