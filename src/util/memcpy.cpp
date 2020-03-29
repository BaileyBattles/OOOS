#include "util/memcpy.h"

int memcpy(void* from, void* to, int size){
    for (int i = 0; i < size; i++){ 
        *(char*)(to + i) = *(char*)(from + i);
    }
    return 0;
}