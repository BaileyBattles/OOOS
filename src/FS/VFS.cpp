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
    //fileSystems[0]->mkdir("/BIN");
    //fileSystems[0]->mkfile("/BIN/FILE");
    fileSystems[0]->ls("/BIN");
    fileSystems[0]->mkfile("/BIN/FILE2");
    char buffer[] = "bbaaaaaaaa";
    fileSystems[0]->writeNBytes("/BIN/FILE2", buffer, 3);
    char buffer2[13];
    buffer2[12] = '\0';
    fileSystems[0]->readNBytes("/BIN/FILE2", buffer2, 12);
    kprint(buffer2);
}