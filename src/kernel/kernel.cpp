#include "drivers/screen.h"
#include "sys/interrupt.h"
#include "kernel/types.h"
#include "util/atoi.h"


extern "C" void kernelMain() {
    clear_screen();
    InterruptManager interruptManager;
    asm volatile ("int $0x0");
    while(true){
    }
}


