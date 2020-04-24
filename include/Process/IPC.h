#ifndef __IPC_H__
#define __IPC_H__

#include "Kernel/Types.h"

#define IPC_BUFFER_SIZE 1024

class Process;

class IPCSocket {
public:
    IPCSocket();
    int connectTo(Process* process);
    int write(const char* buffer, int nbytes);
    int read(char* buffer, int nbytes);
private:
    u8 buffer[IPC_BUFFER_SIZE + 1];
    IPCSocket *connection;
    int getBytes(char *buffer, int nbytes);


    bool start;
    int writePtr;
    int readPtr;
    bool bufferFull();
    bool bufferEmpty();
    int writeToBuffer(u8 byte);
    int readFromBuffer(u8 &byte);
};

#endif