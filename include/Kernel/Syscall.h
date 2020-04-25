#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "CPU/InterruptHandlingObject.h"
#include "Drivers/Screen.h"

#define NUM_SYSCALLS 10

//Sycalls
int printf(char *buffer);
int getInput(char *buffer);

void theRealGetInput(char *buffer);

//Syscall Wrappers
int _kprint(char* buffer, char* result);
int _getInput(char* buffer, char* result);

typedef int (*syscall_t)(char *, char*);
static syscall_t _syscall_table[] {
    _kprint,
    _getInput
};


///////////////////
// SysCall Table //
///////////////////

int call_syscall(int num, void *args,  void *results);

#endif
