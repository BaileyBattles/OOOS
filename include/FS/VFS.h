#ifndef __VIRTUALFILESYSTEM_H__
#define __VIRTUALFILESYSTEM_H__

#include "FS/FileSystem.h"

#define MAX_NUM_FILESYSTEMS 4

#define VFS VirtualFileSystem::the()

class VirtualFileSystem {
public:    
    VirtualFileSystem();

    static VirtualFileSystem& the()
    {
        static VirtualFileSystem instance; // Guaranteed to be destroyed.
                                // Instantiated on first use.
        return instance;
    }

    void mount(FileSystem &fileSystem, char name);

private:
    char names[MAX_NUM_FILESYSTEMS];
    FileSystem *fileSystems[MAX_NUM_FILESYSTEMS];
    int currNumFileSystems;
};

#endif