#include "central_piece.h"

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

void
RenderWeirdGradient(offscreen_buffer *Backbuffer, int BlueOffset, int GreenOffset){
    uint8 *Row = (uint8 *)Backbuffer->Memory;
    for(int Y = 0;Y<Backbuffer->Height;++Y){
        uint32 *Pixel = (uint32*)Row;
        for(int X = 0;X<Backbuffer->Width;++X){
            uint8 Blue  = (X + BlueOffset);
            uint8 Green = (Y + GreenOffset);
            *Pixel++    = ((Green << 8) | Blue);
        }
        Row += Backbuffer->Pitch;
    }
}

void UpdateAndRender(offscreen_buffer *screen){
	static int x = 0;
	static int y = 0;
	RenderWeirdGradient(screen, x++, y++);
}

