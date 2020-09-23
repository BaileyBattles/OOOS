#ifndef __SYSCALL_H__
#define __SYSCALL_H__

//Syscall Wrappers
int _kprint(char* buffer, char* result);
int _getInput(char* buffer, char* result);
int _fork(char* buffer, char* result);
int _exec(char* buffer, char* result);
int _ls(char* buffer, char* result);
int _exit(char* buffer, char *result);
int _yield(char* buffer, char* result);

typedef int (*syscall_t)(char *, char*);
static syscall_t _syscall_table[] {
    _kprint,
    _getInput,
    _fork,
    _exec,
    _ls,
    _exit,
    _yield
};


#endif