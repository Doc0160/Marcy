#if !defined(PLATFORM_H)

#define internal static
#define global static
#define persist static

#define Assert(x) \
	do { if (!(x)) { __debugbreak(); } } while(0)

#define DINFO(msg) OutputDebugStringA("[ ] "msg"\n");
#define DFAIL(msg) OutputDebugStringA("[-] "msg"\n");
#define DSUCCESS(msg) OutputDebugStringA("[+] "msg"\n");

#include <stdint.h>
//
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
//
typedef int32 bool32;
//
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
//
typedef float real32;
typedef double real64;
//

#define PLATFORM_H
#endif