#ifndef __SYSCALL_H__
#define __SYSCALL_H__

//Syscall Wrappers
int _kprint(char* buffer, char* result);
int _getInput(char* buffer, char* result);
int _fork(char* buffer, char* result);
int _exec(char* buffer, char* result);

typedef int (*syscall_t)(char *, char*);
static syscall_t _syscall_table[] {
    _kprint,
    _getInput,
    _fork,
    _exec
};


#endif