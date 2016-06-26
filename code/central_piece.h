#if !defined(CENTRAL_H)
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
#define Assert(x) do { if (!(x)) { *(int *)0=0; } } while(0)
#else
#define Assert(x)
#endif
/*#define Assert(x) \
	do { if (!(x)) { __debugbreak(); } } while(0)*/
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
// TODO(doc): 64 bits ???
#define Kilobytes(value) ((value)*1024)
#define Megabytes(value) (Kilobytes(value)*1024)
#define Gigabytes(value) (Megabytes(value)*1024)
#define Terabytes(value) (Gigabytes(value)*1024)

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
//
//
//
struct state{
	int BlueOffset;
	int GreenOffset;
	int RedOffset;
};
//
struct clocks{
	// TODO
	real32 SecondsElapsed;
};
// timing ?, keyboard/mouse input, bitmap buffer to use
void UpdateAndRender(memory *Memory, input *Input, offscreen_buffer *Screen);

#define PLATFORM_H
#endif