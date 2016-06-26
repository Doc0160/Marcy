#include "central_piece.h"

internal void
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

internal void
UpdateAndRender(input *Input, offscreen_buffer *Screen){
	persist int x = 0;
	persist int y = 0;
	if(Input->Up.EndedDown){
		y++;
	}else if(Input->Down.EndedDown){
		y--;
	}
	if(Input->Left.EndedDown){
		x++;
	}else if(Input->Right.EndedDown){
		x--;
	}
	RenderWeirdGradient(Screen, x, y);
}

