#include "cpu/interrupt_manager.h"
#include "drivers/ide.h"
#include "drivers/keyboard.h"
#include "drivers/pci.h"
#include "drivers/screen.h"
#include "kernel/multiboot.h"
#include "memory/kMemoryManger.h"
#include "memory/paging.h"
#include "util/string.h"

//Global Variables
extern unsigned long ebss;

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;


// Kernel Helper Functions
void kernelLoop() {
    while (true)
    {}
}

void testInterrupts() {
    //__asm__ __volatile__("int $2");
    //__asm__ __volatile__("int $3");
}

void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

void initializeKMM() {
    u32 endOfBss = (u32)&ebss;
    KMM.initialize(endOfBss);
}

void testKMM() {
    char *buffer = (char*)KMM.kmalloc(32);
    if (buffer == nullptr){
        kprint("Failed to initialize buffer");
    }
    char *buffer2 = (char*)KMM.kmalloc(32);
    if (buffer == buffer2) {
        kprint("KMM allocated buffer == buffer2");
    }
}


////////////////
// kernelMain //
////////////////



extern "C" void kernelMain(multiboot_header_t* multibootHeader) {
    clear_screen();

    callConstructors();
    
    initializeKMM();
    testKMM();

    InterruptManager interruptManager;
    Keyboard keyboard;
    keyboard.initialize();

    PTM.initialize();

    PCI::the().scan();

    IDE ide;
    ide.initialize();

    kernelLoop();

}

