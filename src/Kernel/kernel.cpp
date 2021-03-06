#include "CPU/Gdt.h"
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
#include "Kernel/Syscall.h"
#include "Memory/KMemoryManager.h"
#include "Memory/Paging.h"
#include "Process/Process.h"
#include "Process/Scheduler.h"
#include "Util/String.h"

//Global Variables
//extern unsigned long ebss;

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" unsigned long ebss;
extern "C" unsigned long e_stack;

extern "C" void jump_usermode();

void stage2(Process *initProcess);

Keyboard keyboard;


void testInterrupts() {
    //__asm__ __volatile__("int $13");
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

void causeGPF() {
    kprint("HERE");
}

// Kernel Helper Functions
extern "C"
void kernelLoop() {
    causeGPF();
    while (true)
    {}
}

////////////////
// kernelMain //
//////////////// 



extern "C" void kernelMain(multiboot_header_t* multibootHeader) {
    u32 address = (u32)&e_stack;
    address -= 7304;
    asm volatile("movl %%eax, %%esp" ::"a"(address)
                 : "memory");
    
    asm volatile("movl %%eax, %%ebp" ::"a"(&e_stack)
                 : "memory");

    clear_screen();

    callConstructors();

    initializeKMM();
    testKMM();

    InterruptManager interruptManager;

    keyboard.initialize();


    PTM.initialize();
    
    gdt_init(); //this crashes things if you move it around...

    IDE ide0(IDE0_PORT);
    ide0.initialize();

    FAT16 f16(ide0);
    FileSystem *fileSystem = (FileSystem*)&f16;

    VFS.mount(*fileSystem, 'A');

    Process init = Process::createInitProcess(stage2);
    Scheduler::the().runNext();
    while (true) {
        kprint("Init Process didn't get started");
    }
}

void stage2(Process *initProcess) {
    int pid = initProcess->fork();
    if (pid == 0) {
        Scheduler::the().runningProcess()->connectToKeyboard(&keyboard);
        Scheduler::the().runningProcess()->exec("/BIN/SH");
    }
    initProcess->exit();
    Scheduler::the().runNext();
    kprint("here");

    kernelLoop();
}