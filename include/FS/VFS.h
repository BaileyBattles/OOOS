#ifndef __VIRTUALFILESYSTEM_H__
#define __VIRTUALFILESYSTEM_H__

#include "FS/FileSystem.h"
#include "Lib/KVector.h"

#define MAX_NUM_FILESYSTEMS 4

typedef struct {
    const char* path;
    int fileSystem;
} FileInfo;

#define VFS VirtualFileSystem::the()

class VirtualFileSystem {
public:  
    static VirtualFileSystem& the()
    {
        static VirtualFileSystem instance; // Guaranteed to be destroyed.
                                // Instantiated on first use.
        return instance;
    }
    void mount(FileSystem &fileSystem, char name);
    int open(const char path[], int flags);
    int read(int fd, char buffer[], int nbytes);
    int write(int fd, char buffer[], int nbytes);
    int close(int fd);
    int ls(const char path[]);

private:
    VirtualFileSystem();
    char names[MAX_NUM_FILESYSTEMS];
    FileSystem *fileSystems[MAX_NUM_FILESYSTEMS];
    int currNumFileSystems;
    KVector<FileInfo> fdTable;
};

#endif