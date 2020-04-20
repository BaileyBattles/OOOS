#include "Drivers/Screen.h"

int main() {
    char *ptr = (char *)0x2000000;
    ptr[0] = 'H';
    ptr[1] = 'e';
    ptr[2] = 'l';
    char data[] = "Hello";
    kprint(data);
}