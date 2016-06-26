#include "central_piece.h"

internal void
RenderWeirdGradient(offscreen_buffer *Backbuffer, int BlueOffset, int GreenOffset, int RedOffset){
    uint8 *Row = (uint8 *)Backbuffer->Memory;
    for(int Y = 0;Y<Backbuffer->Height;++Y){
        uint32 *Pixel = (uint32*)Row;
        for(int X = 0;X<Backbuffer->Width;++X){
            uint8 Blue  = (X + BlueOffset);
            uint8 Green = (Y + GreenOffset);
            uint8 Red = (RedOffset);
            *Pixel++    = ((Red << 16) |(Green << 8) | Blue);
        }
        Row += Backbuffer->Pitch;
    }
}

internal void
UpdateAndRender(memory *Memory, input *Input, offscreen_buffer *Screen){
	Assert(sizeof(state) <= Memory->PermanentStorageSize);
	state *State = (state *)Memory->PermanentStorage;
	if(!Memory->IsInitialized){
		char *Filename = __FILE__;
		DEBUG_read_file_result File = DEBUGPlatformReadEntireFile(Filename);
		if(File.Contents){
			DEBUGPlatformWriteEntireFile("D:\\_C\\Marcy\\test.out", 
				File.ContentsSize, File.Contents);
			DEBUGPlatformFreeFileMemory(File.Contents);
		}
		//
		//TODO(doc): may be more appropriate to do in the platform layer
		Memory->IsInitialized=1;
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
	if(Input->Enter.EndedDown){
		State->RedOffset=0;
	}else if(Input->Back.EndedDown){
		State->RedOffset=255;
	}
	RenderWeirdGradient(Screen, State->GreenOffset, State->BlueOffset, State->RedOffset);
}

