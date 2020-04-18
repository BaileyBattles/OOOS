#include "Drivers/Screen.h"

extern "C"
void __cxa_pure_virtual (void) {
    kprint("Pure virtual method called\n");
    while (true)
        ;
}