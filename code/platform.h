#if !defined(PLATFORM_H)

#define internal static
#define global static
#define persist static

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
/*
	NOTE(doc):
		MARCY_SLOW:
			0 - no slow code allowed
			1 - slow code welcome
		MARCY_DEBUG:
			0 - dev code
			1 - public release
*/
#if MARCY_SLOW
#define Assert(x) if(!(x)){ *(int *)0=0; }
// #define Assert(x) do { if (!(x)) { __debugbreak(); } } while(0)
#else
#define Assert(x)
#endif
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
// TODO(doc): 64 bits ???
#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)
#if MARCY_DEBUG
struct DEBUG_read_file_result{
	uint32 ContentsSize;
	void *Contents;
};
internal DEBUG_read_file_result DEBUGPlatformReadEntireFile(char *Filename);
internal void DEBUGPlatformFreeFileMemory(void *Memory);
//
internal bool32
DEBUGPlatformWriteEntireFile(char *Filename, uint32 MemorySize, void *Memory);
#endif
//
inline uint32
SafeTruncateUInt64(uint64 value){
	//TODO(doc): define max values
	Assert(value <= 0xFFFFFFFF);
	uint32 value32 = (uint32)value;
	return value32;
}
// NOTE(doc): may cause platform inefficiency
struct offscreen_buffer{
	void       *Memory;
	int        Width;
	int        Height;
	int        Pitch;
};
//
struct input_button{
	int HalfTransitionCount;
	bool32 EndedDown;
};
struct input{
	union{
		input_button Buttons[6];
		struct{
			input_button Up;
			input_button Down;
			input_button Left;
			input_button Right;
			input_button Enter;
			input_button Back;
		};
	};
};
//
struct memory{
	bool32 IsInitialized;
	uint64 PermanentStorageSize;
	void *PermanentStorage; // NOTE(doc): REQUIRED to be cleared to 0
	uint64 TransientStorageSize;
	void *TransientStorage; // NOTE(doc): REQUIRED to be cleared to 0
};
// timing ?, keyboard/mouse input, bitmap buffer to use
internal void 
UpdateAndRender(memory *Memory, input *Input, offscreen_buffer *Screen);
//
//
//
struct state{
	int BlueOffset;
	int GreenOffset;
	int RedOffset;
};
#define PLATFORM_H
#endif