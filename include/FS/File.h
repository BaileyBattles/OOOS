#ifndef __FILE_H__
#define __FILE_H__

#include "FS/FileInfo.h"

class FileSystem;



class File {
public:
    File(FileSystem &theFileSystem, FileInfo theFileInfo);
    int read(char buffer[], int nbytes);
private:
    FileSystem* fileSystem;
    FileInfo fileInfo;
    int fileSystemSectorSize;
};

#endif