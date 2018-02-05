#ifndef EAGLEYE_EAGLDEFINES_H
#define EAGLEYE_EAGLDEFINES_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];

#define PACK __attribute__((__packed__))

// BIN_ID: Strips the high byte of a chunk ID (read as little-endian integers)
#define BIN_ID(n) ((DWORD) (n) & 0xFFFFFF)
#define LOWORD(a) ((WORD)(a))
#define HIWORD(a) ((WORD)(((DWORD)(a) >> 16) & 0xFFFF))

#endif //EAGLEYE_EAGLDEFINES_H
