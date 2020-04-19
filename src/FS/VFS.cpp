#include "Drivers/Screen.h"
#include "FS/VFS.h"
#include "Util/String.h"

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
    File file = fileSystems[0]->getFile("/Dir1/Dir2");
    char buffer[] = "My first file";
    fileSystems[0]->writeNBytes(file, buffer, strlen(buffer));
    char buffer2[14];
    fileSystems[0]->readNBytes(file, buffer2, 14);
}