#if !defined(CENTRAL_H)

struct offscreen_buffer{
	void       *Memory;
	int        Width;
	int        Height;
	int        Pitch;
};
/* timing ?, keyboard/mouse input, bitmap buffer to use */
void UpdateAndRender(offscreen_buffer *screen);

#define PLATFORM_H
#endif