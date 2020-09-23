#ifndef __STRING_H__
#define __STRING_H__

char *int_to_ascii(int n, char buffer[]);
int strlen(const char buff[]);
void strcpy(const char source[], char dest[]);
//Returns -1 if string 1 < string 2
//Returns 0 if they are equal
//Returns 1 if string 1 > string2
int strCmp(const char string1[], const char string2[]);

#endif
