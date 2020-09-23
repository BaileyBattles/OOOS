#ifndef TYPES_H
#define TYPES_H

/* Instead of using 'chars' to allocate non-character bytes,
 * we will use these new type with no semantic meaning */
typedef unsigned long long u64;
typedef unsigned int       u32;
typedef          int       s32;
typedef unsigned short     u16;
typedef          short     s16;
typedef unsigned char      u8;
typedef          char      s8;

typedef unsigned long      size_t;

#define low_16(address) (u16)((address) & 0xFFFF)
#define high_16(address) (u16)(((address) >> 16) & 0xFFFF)

#define KB 1024
#define MB 1048576
#define GB 1073741824

#define NULL 0

#define IDE0_PORT 0x1F0

#endif
