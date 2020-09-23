#include "Drivers/Screen.h"

//Will need to implement C++ ABI someday

extern "C"
void __cxa_pure_virtual (void) {
    kprint("Pure virtual method called\n");
    while (true)
        ;
}

void *__dso_handle;

extern "C"
int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle) 
{}