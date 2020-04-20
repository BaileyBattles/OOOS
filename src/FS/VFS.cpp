#include "Drivers/Screen.h"
#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Util/String.h"

VirtualFileSystem::VirtualFileSystem() {
    currNumFileSystems = 0;
}

int VirtualFileSystem::open(const char path[], int flags){
    for (int i = 0; i < fdTable.size(); i++) {
        if (strCmp(fdTable.get(i).path, path) == 0) {
            return i;
        }
    }
    //We can determine this based on the mount points
    //but that is a project for the future
    int fileSystem = 0;
    fileSystems[fileSystem]->mkfile(path);
    FileInfo info;
    info.fileSystem = fileSystem;
    info.path = path;
    fdTable.push(info);
    return fdTable.size() - 1;
}

int VirtualFileSystem::read(int fd, char buffer[], int nbytes) {
    if (fd >= fdTable.size()) {
        return -1;
    }
    FileInfo info = fdTable.get(fd);
    return fileSystems[info.fileSystem]->readNBytes(info.path, buffer, nbytes);
}

int VirtualFileSystem::write(int fd, char buffer[], int nbytes) {
    if (fd >= fdTable.size()) {
        return -1;
    }
    FileInfo info = fdTable.get(fd);
    return fileSystems[info.fileSystem]->writeNBytes(info.path, buffer, nbytes);
}

int VirtualFileSystem::close(int fd) {
    
}

int VirtualFileSystem::ls(const char path[]) {
    int fileSystem = 0;
    return fileSystems[0]->ls(path);
}


void VirtualFileSystem::mount(FileSystem& fileSystem, char name) {
    if (currNumFileSystems == MAX_NUM_FILESYSTEMS) {
        kprint("Already have the max number of filesystems!  Please eject\n");
        return;
    } 
    names[currNumFileSystems] = name;
    fileSystems[currNumFileSystems] = &fileSystem;
    currNumFileSystems++;
}