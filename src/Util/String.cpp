#include "Memory/KMemoryManager.h"
#include "Util/String.h"

char *int_to_ascii(int num, char buffer[]) {
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

int strlen(const char buff[]) {
    int len = 0;
    for(; *buff != '\0'; buff++)
        len++;
    return len;
}

int strCmp(const char string1[], const char string2[]) {
    int len1 = strlen(string1);
    int len2 = strlen(string2);
    int minLen = len1;
    if (len2 < len1)
        minLen = len2;
    for (int i = 0; i < minLen; i++) {
        if (string1[i] > string2[i]) {
            return 1;
        }
        else if (string2[i] > string1[i]) {
            return -1;
        }
    }
    if (len1 > len2)
        return 1;
    else if (len2 > len1){
        return -1;
    }
    return 0;
}