#if !defined(WIN32_PLATFORM_H)
//
#define DINFO(msg) \
	OutputDebugStringA("[ ] "msg"\n");
#define DFAIL(msg) \
	OutputDebugStringA("[-] "msg"\n");
#define DSUCCESS(msg) \
	OutputDebugStringA("[+] "msg"\n");
//
#include <windows.h>
#include <stdio.h>
//
#undef OutputDebugString
#undef RegisterClass
#undef DefWindowProc
#undef CreateWindowEx
#undef GetMessage
#undef DispatchMessage
#undef WNDCLASS
#undef MessageBox
#undef LoadLibrary
#undef CreateFile
#undef CopyFile
//
struct window_dimension{
	int Width;
	int Height;
};
//
struct win32_offscreen_buffer{
	// NOTE(doc): pixels are alwasy 32-bits wide, memory order BB GG RR XX
	BITMAPINFO Info;
	void       *Memory;
	int        Width;
	int        Height;
	int        Pitch;
};
#define WIN32_PLATFORM_H
#endif