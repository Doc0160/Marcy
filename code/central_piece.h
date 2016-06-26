#if !defined(CENTRAL_H)

#define ArrayCount(Array) \
	(sizeof(Array) / sizeof((Array)[0]))
#define MIN(x, y) \
	((x) <= (y) ? (x) : (y))
#define MAX(x, y) \
	((x) >= (y) ? (x) : (y))
#define Kilobytes(value) \
	((value)*1024)
#define Megabytes(value) \
	(Kilobytes(value)*1024)
#define Gigabytes(value) \
	(Megabytes(value)*1024)

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
	void *PermanentStorage;
};
//
//
//
struct state{
	int BlueOffset;
	int GreenOffset;
};
// timing ?, keyboard/mouse input, bitmap buffer to use
void UpdateAndRender(memory *Memory, input *Input, offscreen_buffer *Screen);

#define PLATFORM_H
#endif