#include "util/atoi.h"

//TODO: Implement
int atoi(char *buffer){
    return 0;
}

//TODO: Reimplement when I have malloc
char* itoa(int num){
    char *buffer = (char *)0x8ffc4;
    if (num == 0) {
        buffer[0] = 0x30;
        buffer[1] = '\0';
        return buffer;
    }
    int len = 0;
    while (num > 0){
        buffer[len] = 0x30 + num % 10;
        len++;
        num /= 10;
    }
    int i;
    for (i = 0; i < len / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = temp;
    }
    buffer[len] = '\0';
    return buffer;
}