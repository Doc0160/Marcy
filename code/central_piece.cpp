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
UpdateAndRender(memory *Memory, input *Input, offscreen_buffer *Screen){
	state *State = (state *)Memory->PermanentStorage;
	if(!Memory->IsInitialized){
		State->BlueOffset = 0;
		State->GreenOffset = 0;
	}
	if(Input->Up.EndedDown){
		State->BlueOffset++;
	}else if(Input->Down.EndedDown){
		State->BlueOffset--;
	}
	if(Input->Left.EndedDown){
		State->GreenOffset++;
	}else if(Input->Right.EndedDown){
		State->GreenOffset--;
	}
	RenderWeirdGradient(Screen, State->GreenOffset, State->BlueOffset);
}

