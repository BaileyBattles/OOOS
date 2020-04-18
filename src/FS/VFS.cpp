#include "Drivers/Screen.h"
#include "FS/VFS.h"

VirtualFileSystem::VirtualFileSystem() {
    currNumFileSystems = 0;
}

void VirtualFileSystem::mount(FileSystem& fileSystem, char name) {
    if (currNumFileSystems == MAX_NUM_FILESYSTEMS) {
        kprint("Already have the max number of filesystems!  Please eject\n");
        return;
    } 
    names[currNumFileSystems] = name;
    fileSystems[currNumFileSystems] = &fileSystem;
    currNumFileSystems++;
    fileSystems[0]->ls("/Dir1");
}