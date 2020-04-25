#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "CPU/InterruptHandlingObject.h"
#include "Drivers/Screen.h"

class SyscallHandler : public InterruptHandlingObject {
public:
    virtual void initialize();
    virtual void handleInterrupt(registers_t r);
private:
};

typedef void (*syscall_t)(const char *);
///////////////////
// SysCall Table //
///////////////////

void call_syscall(void *args);

#endif
