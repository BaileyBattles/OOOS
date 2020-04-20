#include "Drivers/Screen.h"
#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
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
    //fileSystems[0]->writeNBytes("/BIN/FILE", buffer, 3);
    char buffer2[13];
    buffer2[12] = '\0';
    //kprint(buffer2);
    char *buff = (char*)0x1000000;
    fileSystems[0]->readNBytes("/BIN/FILE", buff, 12652);

}