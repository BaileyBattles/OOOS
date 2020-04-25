#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "CPU/InterruptHandlingObject.h"
#include "Drivers/Screen.h"

#define NUM_SYSCALLS 10

//Sycalls
void printf(char *buffer);
void getInput(char *buffer);

void theRealGetInput(char *buffer);

//Syscall Wrappers
void _kprint(char* buffer, char* result);
void _getInput(char* buffer, char* result);

typedef void (*syscall_t)(char *, char*);
static syscall_t _syscall_table[] {
    _kprint,
    _getInput
};


///////////////////
// SysCall Table //
///////////////////

void call_syscall(int num, void *args,  void *results);

#endif
