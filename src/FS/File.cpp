#include "FS/File.h"
#include "FS/FileSystem.h"

File::File(FileSystem &theFileSystem, FileInfo theFileInfo) {
    fileSystem = &theFileSystem;
    fileInfo = theFileInfo;
}

int File::read(char buffer[], int nbytes) {
    int bytesToRead = nbytes < fileInfo.size ? nbytes : fileInfo.size;
    //fileSystem->readNBytes(fileInfo.startSector, buffer);
}
