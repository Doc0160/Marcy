#if !defined(CENTRAL_H)
#define ArrayCount(Array) \
	(sizeof(Array) / sizeof((Array)[0]))
#define MIN(x, y) \
	((x) <= (y) ? (x) : (y))
#define MAX(x, y) \
	((x) >= (y) ? (x) : (y))
// NOTE(doc): may cause platform inefficiency
struct offscreen_buffer{
	void       *Memory;
	int        Width;
	int        Height;
	int        Pitch;
};
struct input_button{
	int HalfTransitionCount;
	bool EndedDown;
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
// timing ?, keyboard/mouse input, bitmap buffer to use
void UpdateAndRender(input *Input, offscreen_buffer *Screen);

#define PLATFORM_H
#endif