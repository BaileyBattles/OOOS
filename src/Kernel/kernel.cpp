#include "CPU/InterruptManager.h"
#include "Drivers/FileDevice.h"
#include "Drivers/IDE.h"
#include "Drivers/Keyboard.h"
#include "Drivers/PCI.h"
#include "Drivers/Screen.h"
#include "FS/FAT16.h"
#include "FS/FileSystem.h"
#include "FS/VFS.h"
#include "Kernel/Multiboot.h"
#include "Memory/KMemoryManager.h"
#include "Memory/Paging.h"
#include "Process/Process.h"
#include "Util/String.h"

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

    IDE ide0(IDE0_PORT);
    ide0.initialize();

    FAT16 f16(ide0);
    FileSystem *fileSystem = (FileSystem*)&f16;

    VFS.mount(*fileSystem, 'A');

    Process p1("/BIN/FILE");
    p1.exec();
    kprint("here");

    kernelLoop();
}

