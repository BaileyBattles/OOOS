#ifndef __SYSCALL_STUBS_H__
#define __SYSCALL_STUBS_H__

#define NUM_SYSCALLS 10

//Sycalls
int printf(char *buffer);
int getInput(char *buffer);
int fork();
int exec(char *buffer);
int ls();
int exit();

void theRealGetInput(char *buffer);



///////////////////
// SysCall Table //
///////////////////

int call_syscall(int num, void *args,  void *results);

#endif
