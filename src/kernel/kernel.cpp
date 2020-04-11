#include "cpu/interrupt_manager.h"
#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "kernel/multiboot.h"
#include "memory/kMemoryManger.h"
#include "memory/paging.h"
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

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void main(multiboot_header_t* multibootHeader) {
    clear_screen();

    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
    
    u32 endOfBss = (u32)&ebss;
    KMM.initialize(endOfBss);

    InterruptManager interruptManager;

    Keyboard keyboard;
    keyboard.initialize();

    //Idealy don't want KMM on stack, should be in BSS I think
    char *buffer = (char*)KMM.kmalloc(32);
    if (buffer == nullptr){
        kprint("Failed to initialize buffer");
    }
    char *buffer2 = (char*)KMM.kmalloc(32);

    PageTableManager PTM;
    PTM.initialize();
    kernelLoop();

}

