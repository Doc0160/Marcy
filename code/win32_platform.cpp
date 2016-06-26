#include "platform.h"
#include "win32_platform.h"
#include "central_piece.cpp"
/*
	NOT FINAL PLATFORM LAYER
*/
//
void Debugf(char *format, ...){
	char temp[1024];
	va_list arg_list;
	va_start(arg_list, format);
	wvsprintfA(temp, format, arg_list);
	va_end(arg_list);
	OutputDebugStringA(temp);
}
//
global bool32 GlobalRunning = 0;
global win32_offscreen_buffer GlobalBackbuffer = {};
global input GlobalInput = {};
//
internal DEBUG_read_file_result
DEBUGPlatformReadEntireFile(char *Filename){
	DEBUG_read_file_result Result = {};
	HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE){
		LARGE_INTEGER FileSize;
		if(GetFileSizeEx(FileHandle, &FileSize)){
			uint32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
			Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			if(Result.Contents){
				DWORD BytesRead;
				if(ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0)
					&& (FileSize32==BytesRead)){
					//NOTE(doc): success
					Result.ContentsSize = FileSize32;
				}else{
					//TODO
					DEBUGPlatformFreeFileMemory(Result.Contents);
					Result.Contents=0;
				}
			}else{
				//TODO
			}
		}else{
			//TODO
		}
		CloseHandle(FileHandle);
	}else{
		//TODO
	}
	return Result;
}
internal void
DEBUGPlatformFreeFileMemory(void *Memory){
	if(Memory){
		VirtualFree(Memory, 0, MEM_RELEASE);
	}
}
//
internal bool32
DEBUGPlatformWriteEntireFile(char *Filename, uint32 MemorySize, void *Memory){
	bool32 Result = 0;
	HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE,
		0, 0, CREATE_ALWAYS, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE){
		DWORD BytesWritten;
		if(WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0)
			&& (MemorySize==BytesWritten)){
			//NOTE(doc): success
			Result = 1;
		}else{
			//TODO
		}
		CloseHandle(FileHandle);
	}else{
		//TODO
	}
	return(Result);
}
//
internal window_dimension
win32_GetWindowDimension(
	HWND wnd
){
	window_dimension Result;
	RECT ClientRect;
	GetClientRect(wnd, &ClientRect);
	Result.Width  = ClientRect.right-ClientRect.left;
	Result.Height = ClientRect.bottom-ClientRect.top;
	return(Result);
}
internal void
win32_ResizeDIBSelection(
	win32_offscreen_buffer *Backbuffer,
	int Width, 
	int Height
){
	// TODO(doc): don't free first, free after then free first if that fails
	if(Backbuffer->Memory){
		VirtualFree(Backbuffer->Memory,0,MEM_RELEASE);
		DINFO("Backbuffer freed");
	}
	//
	Backbuffer->Height=Height;
	Backbuffer->Width=Width;
	//
	Backbuffer->Info.bmiHeader.biSize = sizeof(Backbuffer->Info.bmiHeader);
	Backbuffer->Info.bmiHeader.biWidth = Backbuffer->Width;
	// NOTE(doc): if negative, window treat this bitmap as top-down not bottom-up
	// so 1st 3 bytes = top-left
	Backbuffer->Info.bmiHeader.biHeight = -Backbuffer->Height;
	Backbuffer->Info.bmiHeader.biPlanes = 1;
	Backbuffer->Info.bmiHeader.biBitCount = 32;
	Backbuffer->Info.bmiHeader.biCompression = BI_RGB;
	//
	int BitmapMemorySize = (Backbuffer->Width*Backbuffer->Height)*4;
	Backbuffer->Memory = VirtualAlloc(0, BitmapMemorySize, 
		MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
	Backbuffer->Pitch = Backbuffer->Width*4;
	DINFO("Backbuffer allocated");
	// TODO(doc): maybe init to black
}
internal void
win32_UpdateWindow(
	HDC DeviceContext, 
	win32_offscreen_buffer *Backbuffer, 
	int Width, 
	int Height
){
	StretchDIBits(DeviceContext,
		0, 0, Width,             Height,             // to
		0, 0, Backbuffer->Width, Backbuffer->Height, // from
		Backbuffer->Memory, &Backbuffer->Info,
		DIB_RGB_COLORS, SRCCOPY);
}
internal LRESULT CALLBACK
WindowProc(
	HWND   wnd,
	UINT   Msg,
	WPARAM WParam,
	LPARAM LParam
){
	LRESULT Result = 0;
	switch(Msg){
		case WM_CLOSE:{
			GlobalRunning=0;
		}break;
		case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
		case WM_KEYUP:
		case WM_KEYDOWN:{
			uint32 VKCode = WParam;
            bool32 WasDown = ((LParam & (1 << 30)) != 0);
            bool32 IsDown = ((LParam & (1 << 31)) == 0);
			bool32 AltKeyWasDown = (LParam & (1 << 29));
			switch(VKCode){
				case VK_LEFT:
				case 'Q':{
					GlobalInput.Left.HalfTransitionCount=1;
					GlobalInput.Left.EndedDown=IsDown;
				} break;
				case VK_RIGHT:
				case 'D':{
					GlobalInput.Right.HalfTransitionCount=1;
					GlobalInput.Right.EndedDown=IsDown;
				} break;
				case VK_UP:
				case 'Z':{
					GlobalInput.Up.HalfTransitionCount=1;
					GlobalInput.Up.EndedDown=IsDown;
				} break;
				case VK_DOWN:
				case 'S':{
					GlobalInput.Down.HalfTransitionCount=1;
					GlobalInput.Down.EndedDown=IsDown;
				} break;
				case VK_RETURN:
				case 'E':{
					GlobalInput.Enter.HalfTransitionCount=1;
					GlobalInput.Enter.EndedDown=IsDown;
				} break;
				case VK_BACK:
				case 'A':{
					GlobalInput.Back.HalfTransitionCount=1;
					GlobalInput.Back.EndedDown=IsDown;
				} break;
				default:{
				} break;
			}
			if(WasDown != IsDown){
				switch(VKCode){
					case VK_ESCAPE:{
						GlobalRunning=false;
					} break;
					case VK_F4:{
						if(AltKeyWasDown){
							GlobalRunning=false;
						}
					} break;
					default:{
					} break;
				}
			}
		} break;
		case WM_PAINT:{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(wnd, &Paint);
			window_dimension Dimension = win32_GetWindowDimension(wnd);
			win32_UpdateWindow(DeviceContext, &GlobalBackbuffer,
				Dimension.Width, Dimension.Height);
			EndPaint(wnd, &Paint);
		} break;
		default:{
			// NOTE(doc): let window do his things 
				//'cause some events need specific return values
			Result=DefWindowProcA(wnd,Msg,WParam,LParam);
		} break;
	}
	return(Result);
}
internal int CALLBACK 
WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CmdLine,
	int       CmdShow
){
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
	//
	WNDCLASSA WindowClass = {};
	win32_ResizeDIBSelection(&GlobalBackbuffer, 1280/2, 720/2);
	WindowClass.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance   = Instance;
	// WindowClass.hIcon         = ;
	WindowClass.lpszClassName = "MARCY_WC";
	//
	if(RegisterClassA(&WindowClass)){
		HWND WindowHandle = CreateWindowExA(
			0, "MARCY_WC", "Marcy",
			WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			0, 0, Instance, 0);
		if(WindowHandle){
			GlobalRunning=1;
			HDC DeviceContext = GetDC(WindowHandle);
			//
#if MARCY_DEBUG
			LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
			LPVOID BaseAddress = 0;
#endif
			memory Memory = {}; 
			Memory.PermanentStorageSize = Megabytes(64);
			Memory.TransientStorageSize = Gigabytes(4);
			//
			uint64 TotalSize = Memory.PermanentStorageSize 
				+ Memory.TransientStorageSize;
			Memory.PermanentStorage = VirtualAlloc(BaseAddress, 
				TotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 
			Memory.TransientStorage = ((uint8 *)Memory.PermanentStorage 
				+ Memory.PermanentStorageSize);
			//
			if(Memory.PermanentStorage && Memory.TransientStorage){
				offscreen_buffer Buffer = {};
				Buffer.Memory = GlobalBackbuffer.Memory;
				Buffer.Width = GlobalBackbuffer.Width;
				Buffer.Height = GlobalBackbuffer.Height;
				Buffer.Pitch = GlobalBackbuffer.Pitch;
				//
				LARGE_INTEGER LastCounter;
				QueryPerformanceCounter(&LastCounter);
				int64 LastCycleCount = __rdtsc();
				//
				while(GlobalRunning){
					// windows messages
					MSG Message;
					while(PeekMessageA(&Message,0,0,0,PM_REMOVE)){
						TranslateMessage(&Message);
						DispatchMessageA(&Message);
					}
					UpdateAndRender(&Memory, &GlobalInput, &Buffer);
					//
					window_dimension Dimension = win32_GetWindowDimension(WindowHandle);
					win32_UpdateWindow(
						DeviceContext, &GlobalBackbuffer, 
						Dimension.Width, Dimension.Height
					);
					//
					int64 EndCycleCount = __rdtsc();
					LARGE_INTEGER EndCounter;
					QueryPerformanceCounter(&EndCounter);
					//
#if MARCY_DEBUG
					uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
					int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
					int32 MSPerFrame = (1000 * CounterElapsed) / PerfCountFrequency;
					int32 FPS = PerfCountFrequency / CounterElapsed;
					int32 MCPF = CyclesElapsed / (1000 * 1000);
					Debugf("%dms/f, %df/s, %dMc/f\n", MSPerFrame, FPS, MCPF);
#endif
					//
					LastCounter = EndCounter;
					LastCycleCount = EndCycleCount;
				}
			}else{
				// TODO
				DFAIL("allocation");
			}
			// TODO(doc): maybe delete that, windows do it anyway ~~
			ReleaseDC(WindowHandle, DeviceContext);
		}else{
			// TODO
			DFAIL("window");
		}
	}else{
		// TODO
		DFAIL("register window class");
	}
	return(0);
}