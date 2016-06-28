#include "marcy.h"

internal void
RenderWeirdGradient(offscreen_buffer *Backbuffer, int BlueOffset,
		int GreenOffset, int RedOffset){
    uint8 *Row = (uint8 *)Backbuffer->Memory;
    for(int Y = 0;Y<Backbuffer->Height;++Y){
        uint32 *Pixel = (uint32*)Row;
        for(int X = 0;X<Backbuffer->Width;++X){
            uint8 Blue  = (uint8)(X + BlueOffset);
            uint8 Green = (uint8)(Y + GreenOffset);
            uint8 Red = (uint8)(RedOffset);
            *Pixel++    = ((Red << 16) | (Green << 8) | Blue);
            // *Pixel++    = ((Green << 8) | Blue);
        }
        Row += Backbuffer->Pitch;
    }
}
internal void
RenderPlayer(offscreen_buffer *Backbuffer, int PlayerX, int PlayerY){
	uint8 * EndOfBuffer = (uint8 *)Backbuffer->Memory + 
		Backbuffer->Pitch * (Backbuffer->Height -1);
	uint32 Color = 0xFFFFFFFF;
	for(int X = PlayerX; X < PlayerX + 10; ++X){
		uint8 *Pixel = ((uint8 *)Backbuffer->Memory
						+ X * 4
						+ PlayerY * Backbuffer->Pitch);
		for(int Y = PlayerY; Y < PlayerY + 10; ++Y){
			if((Pixel >= Backbuffer->Memory)
				&& ((Pixel + 4) < EndOfBuffer)){
				*(uint32 *)Pixel = Color;
			}
			Pixel += Backbuffer->Pitch;
		}
	}
}

extern "C" UPDATE_AND_RENDER(UpdateAndRender){
	Assert(sizeof(state) <= Memory->PermanentStorageSize);
	state *State = (state *)Memory->PermanentStorage;
	if(!Memory->IsInitialized){
		Assert((&Input->Keyboard.Back - &Input->Keyboard.Buttons[0]) == 
			(ArrayCount(Input->Keyboard.Buttons) - 1));
		char *Filename = __FILE__;
		DEBUG_read_file_result File = 
			Memory->DEBUGPlatformReadEntireFile(Thread, Filename);
		if(File.Contents){
			Memory->DEBUGPlatformWriteEntireFile(Thread, "test.out", 
				File.ContentsSize, File.Contents);
			Memory->DEBUGPlatformFreeFileMemory(Thread, File.Contents);
		}
		State->PlayerX = 100;
		State->PlayerY = 100;
		//
		//TODO(doc): may be more appropriate to do in the platform layer
		Memory->IsInitialized=1;
	}
	if(Input->Keyboard.Up.EndedDown){
		State->BlueOffset++;
		State->PlayerY-=5;
	}else if(Input->Keyboard.Down.EndedDown){
		State->BlueOffset--;
		State->PlayerY+=5;
	}
	if(Input->Keyboard.Left.EndedDown){
		State->GreenOffset++;
		State->PlayerX-=5;
	}else if(Input->Keyboard.Right.EndedDown){
		State->GreenOffset--;
		State->PlayerX+=5;
	}
	if(Input->Keyboard.Enter.EndedDown){
		// State->RedOffset--;
	}else if(Input->Keyboard.Back.EndedDown){
		// State->RedOffset++;
	}
	RenderWeirdGradient(Screen, State->GreenOffset, 
		State->BlueOffset, State->RedOffset);
	for(int ButtonIndex = 0; 
		ButtonIndex < ArrayCount(Input->Mouse.Buttons);
		++ButtonIndex){
		if(Input->Mouse.Buttons[ButtonIndex].EndedDown){
			RenderPlayer(Screen, 10 + 20 * ButtonIndex, 10);
		}
	}
	RenderPlayer(Screen, State->PlayerX, State->PlayerY);
	RenderPlayer(Screen, Input->Mouse.X, Input->Mouse.Y);
}