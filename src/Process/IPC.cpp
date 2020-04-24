#include "Memory/KMemoryManager.h"
#include "Process/IPC.h"

IPCSocket::IPCSocket() {
    start = true;
    writePtr = 0;
    readPtr = 0;
    connection = nullptr;
}

int IPCSocket::connectTo(Process *process) {
    if (process->socket.connection == nullptr){
        process->socket.connection = this;
        return 0;
    }
    return 1;
}

int IPCSocket::write(const char *buffer, int nbytes) {
    int i = 0;
    while (i < nbytes) {
        int status = writeToBuffer(buffer[i]);
        if (status == 0)
            i++;
    }
    return 0;
}

int IPCSocket::read(char *buffer, int nbytes) {
    connection->getBytes(buffer, nbytes);
}

int IPCSocket::getBytes(char *buffer, int nbytes) {
    int i = 0;
    while (i < nbytes) {
        u8 byte;
        int status = readFromBuffer(byte);
        if (status == 0) {
            buffer[i] = byte;
            i++;
        }
    }
    return 0;
}

bool IPCSocket::bufferFull() {
    if (readPtr == writePtr && !start) {
        start = false;
        return true;
    }
    return false;
}

bool IPCSocket::bufferEmpty() {
    if (writePtr == readPtr) {
        return true;
    }
    return false;
}

int IPCSocket::writeToBuffer(u8 byte) {
    if (bufferFull()) {
        return -1;
    }
    buffer[writePtr] = byte;
    writePtr++;
    writePtr = writePtr % IPC_BUFFER_SIZE;
    return 0;
}

int IPCSocket::readFromBuffer(u8 &byte) {
    if (bufferEmpty()) {
        return -1;
    }
    byte = buffer[readPtr];
    readPtr++;
    readPtr = readPtr % IPC_BUFFER_SIZE;
    return 0;
}