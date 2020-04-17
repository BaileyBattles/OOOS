#ifndef __STRING_H__
#define __STRING_H__

char *int_to_ascii(int n, char buffer[]);
int strlen(char buff[]);
//Returns -1 if string 1 > string 2
//Returns 0 if they are equal
//Returns 1 if string 2 > string1
bool cmpStr(char string1[], char string2[]);

#endif
