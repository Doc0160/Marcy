// #include "platform.h"
#include "marcy.h"
#include "win32_platform.h"
/*
	NOT FINAL PLATFORM LAYER
*/
//
global bool32 GlobalRunning = 0;
global win32_offscreen_buffer GlobalBackbuffer = {};
global int64 GlobalPerfCountFrequency;
//
DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory){
	if(Memory){
		VirtualFree(Memory, 0, MEM_RELEASE);
	}
}
DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile){
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
DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile){
	DEBUG_read_file_result Result = {};
	HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE){
		LARGE_INTEGER FileSize;
		if(GetFileSizeEx(FileHandle, &FileSize)){
			uint32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
			Result.Contents = VirtualAlloc(0, FileSize32, 
				MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			if(Result.Contents){
				DWORD BytesRead;
				if(ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0)
					&& (FileSize32==BytesRead)){
					//NOTE(doc): success
					Result.ContentsSize = FileSize32;
				}else{
					//TODO
					DEBUGPlatformFreeFileMemory(Thread, Result.Contents);
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
inline FILETIME
win32_GetLastWriteTime(char *Filename){
	FILETIME LastWriteTime = {};
	WIN32_FIND_DATA Data;
	if(GetFileAttributesEx(Filename, GetFileExInfoStandard, &Data)){
		LastWriteTime = Data.ftLastWriteTime;
	}
	return(LastWriteTime);
}
internal win32_marcy_code
win32_LoadCode(char *SourceDLLName, char *TempDLLName){
	win32_marcy_code Result = {};
	//
	Result.DLLLastWriteTime = win32_GetLastWriteTime(SourceDLLName);
	CopyFileA(SourceDLLName, TempDLLName, FALSE);
	Result.MarcyCodeDLL = LoadLibraryA(TempDLLName);
	if(Result.MarcyCodeDLL){
		Result.UpdateAndRender = (update_and_render *)
			GetProcAddress(Result.MarcyCodeDLL, "UpdateAndRender");
		Result.IsValid = (Result.UpdateAndRender!=0);
	}
	if(!Result.IsValid){
		Result.UpdateAndRender = 0;
	}
	return(Result);
}
internal void
win32_UnloadCode(win32_marcy_code *Code){
	if(Code->MarcyCodeDLL){
		FreeLibrary(Code->MarcyCodeDLL);
		Code->MarcyCodeDLL = 0;
	}
	Code->IsValid = 0;
	Code->UpdateAndRender = 0;
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
	// StretchDIBits(DeviceContext,
		// 0, 0, Width,             Height,             // to
		// 0, 0, Backbuffer->Width, Backbuffer->Height, // from
		// Backbuffer->Memory, &Backbuffer->Info,
		// DIB_RGB_COLORS, SRCCOPY);
		StretchDIBits(DeviceContext,
		0, 0, Backbuffer->Width, Backbuffer->Height,             // to
		0, 0, Backbuffer->Width, Backbuffer->Height, // from
		Backbuffer->Memory, &Backbuffer->Info,
		DIB_RGB_COLORS, SRCCOPY);
}
internal LRESULT CALLBACK
win32_MainWindowCallback(
	HWND   wnd,
	UINT   Msg,
	WPARAM WParam,
	LPARAM LParam
){
	LRESULT Result = 0;
	switch(Msg){
		case WM_ACTIVATEAPP:{
#if 0
			if(WParam == TRUE){
				SetLayeredWindowAttributes(wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
			}else{
				SetLayeredWindowAttributes(wnd, RGB(0, 0, 0), 128, LWA_ALPHA);
			}
#endif
		} break;
		case WM_CLOSE:{
			GlobalRunning = 0;
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
			/* NOTE(doc): let window do his things 
				'cause some events need specific return values
			*/
			Result=DefWindowProcA(wnd,Msg,WParam,LParam);
		} break;
	}
	return(Result);
}
//
internal void
win32_ProcessKeyMessage(input_button *NewState, bool32 IsDown){
	if(NewState->EndedDown != IsDown){
		NewState->EndedDown = IsDown;
		++NewState->HalfTransitionCount;
	}
}
internal void
win32_ProcessPendingMessages(input *Input){
	MSG Message;
	while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)){
		switch(Message.message){
			case WM_QUIT:{
				GlobalRunning = 0;
			} break;
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYUP:
			case WM_KEYDOWN:{
				uint32 VKCode = (uint32)Message.wParam;
				bool32 WasDown = ((Message.lParam & (1 << 30)) != 0);
				bool32 IsDown = ((Message.lParam & (1 << 31)) == 0);
				if(WasDown != IsDown){
					switch(VKCode){
						case VK_LEFT:
						case 'Q':{
							win32_ProcessKeyMessage(&Input->Keyboard.Left, IsDown);
						} break;
						case VK_RIGHT:
						case 'D':{
							win32_ProcessKeyMessage(&Input->Keyboard.Right, IsDown);
						} break;
						case VK_UP:
						case 'Z':{
							win32_ProcessKeyMessage(&Input->Keyboard.Up, IsDown);
						} break;
						case VK_DOWN:
						case 'S':{
							win32_ProcessKeyMessage(&Input->Keyboard.Down, IsDown);
						} break;
						case VK_RETURN:
						case VK_SPACE:
						case 'E':{
							win32_ProcessKeyMessage(&Input->Keyboard.Enter, IsDown);
						} break;
						case VK_BACK:
						case 'A':{
							win32_ProcessKeyMessage(&Input->Keyboard.Back, IsDown);
						} break;
						case VK_ESCAPE:{
							GlobalRunning = 0;
						} break;
						case VK_F5:{
						} break;
						case VK_F9:{
						} break;
						case VK_F4:{
							bool32 AltKeyWasDown = (Message.lParam & (1 << 29));
							if(AltKeyWasDown){
								GlobalRunning = 0;
							}
						} break;
						default:{
						} break;
					}
				}
			} break;
			default:{
				TranslateMessage(&Message);
				DispatchMessageA(&Message);
			} break;
		}
	}
}
inline LARGE_INTEGER
win32_GetWallClock(){
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return(Result);
}
inline real32
win32_GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End){
	real32 Result =
		((real32)(End.QuadPart - Start.QuadPart) / (real32)GlobalPerfCountFrequency);
	return(Result);
}
//
internal void
CatStrings(size_t SourceACount, char *SourceA,
		size_t SourceBCount, char *SourceB,
		size_t DestCount, char *Dest){
	for(int Index = 0;  Index < SourceACount; ++Index){
		*Dest++ = *SourceA++;
	}
	for(int Index = 0;  Index < SourceBCount; ++Index){
		*Dest++ = *SourceB++;
	}
	*Dest++ = 0;
}
internal int
StringLength(char *String){
	int Count = 0;
	while(*String++){
		++Count;
	}
	return Count;
}
//
internal void
win32_GetEXEFileName(win32_state *State){
	DWORD SizeOfFilename = GetModuleFileNameA(0, State->EXEFileName, 
		sizeof(State->EXEFileName));
	State->OnePastLastEXEFileNameSlash = State->EXEFileName;
	for(char *Scan = State->EXEFileName; *Scan; ++Scan){
		if(*Scan == '\\'){
			State->OnePastLastEXEFileNameSlash = Scan + 1;
		}
	}
}
internal void
win32_BuildEXEPathFileName(win32_state *State, char *Filename,
						int DestCount, char *Dest){
	CatStrings(State->OnePastLastEXEFileNameSlash - State->EXEFileName, 
			State->EXEFileName,
			StringLength(Filename), Filename,
			DestCount, Dest);
}
//
int CALLBACK 
WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CmdLine,
	int       CmdShow
){
	win32_state win32_State = {};
	win32_GetEXEFileName(&win32_State);
	char SourceCodeDLLFullpath[WIN32_STATE_FILE_NAME_COUNT];
	win32_BuildEXEPathFileName(&win32_State, "marcy.dll",
		sizeof(SourceCodeDLLFullpath), SourceCodeDLLFullpath);
	char TempCodeDLLFullpath[WIN32_STATE_FILE_NAME_COUNT];
	win32_BuildEXEPathFileName(&win32_State, "marcy_temp.dll",
		sizeof(TempCodeDLLFullpath), TempCodeDLLFullpath);
	//
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;
	//
	// NOTE(doc): set windows scheduler to wake up everry 1ms! (set scheduler granularyty)
		// so sleep is more precision
	UINT DesiredSchedulerMS = 1;
	bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
	//
	WNDCLASSA WindowClass = {};
	win32_ResizeDIBSelection(&GlobalBackbuffer, 1920/2, 1080/2);
	WindowClass.style       = /*CS_OWNDC |*/ CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = win32_MainWindowCallback;
	WindowClass.hInstance   = Instance;
	// WindowClass.hIcon         = ;
	WindowClass.lpszClassName = "MARCY_WC";
	//
	if(RegisterClassA(&WindowClass)){
		HWND WindowHandle = CreateWindowExA(
			0,// WS_EX_TOPMOST | WS_EX_LAYERED, 
			WindowClass.lpszClassName, "Marcy",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			0, 0, Instance, 0);
		if(WindowHandle){
			HDC DeviceContext = GetDC(WindowHandle);
			//
			int MonitorRefreshHz = 60;
			// TODO(doc): how to get that on windows
			int win32_RefreshRate = GetDeviceCaps(DeviceContext, VREFRESH);
			if(win32_RefreshRate > 0){
				MonitorRefreshHz = win32_RefreshRate;
			}
			real32 UpdateHz = ((real32)MonitorRefreshHz / 2.0f);
			real32 TargetSecondsPerFrame = 1.0f / UpdateHz;
			//
			GlobalRunning=1;
			//
#if MARCY_DEBUG
			LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
			LPVOID BaseAddress = 0;
#endif
			memory Memory = {}; 
			Memory.PermanentStorageSize = Megabytes(64);
			Memory.TransientStorageSize = Gigabytes(1);
			//
			Memory.DEBUGPlatformReadEntireFile = DEBUGPlatformReadEntireFile;
			Memory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
			Memory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;
			//
			uint64 TotalSize = Memory.PermanentStorageSize 
				+ Memory.TransientStorageSize;
			//TODO(doc): | MEM_LARGE_PAGES, call ajust token when not on winxp
			Memory.PermanentStorage = VirtualAlloc(BaseAddress, (size_t)TotalSize,
							MEM_RESERVE | MEM_COMMIT,
							PAGE_READWRITE); 
			Memory.TransientStorage = ((uint8 *)Memory.PermanentStorage 
				+ Memory.PermanentStorageSize);
			//
			if(Memory.PermanentStorage && Memory.TransientStorage){
				input InputData = {};
				input *Input = &InputData;
				//
				LARGE_INTEGER LastCounter = win32_GetWallClock();
				int64 LastCycleCount = __rdtsc();
				//
				win32_marcy_code MarcyCode = win32_LoadCode(SourceCodeDLLFullpath, TempCodeDLLFullpath);
				uint32 LoadCounter = 0;
				while(GlobalRunning){
					//
					FILETIME NewDLLWriteTime = win32_GetLastWriteTime(SourceCodeDLLFullpath);
					if(CompareFileTime(&NewDLLWriteTime, &MarcyCode.DLLLastWriteTime) != 0){
						win32_UnloadCode(&MarcyCode);
						MarcyCode = win32_LoadCode(SourceCodeDLLFullpath, TempCodeDLLFullpath);
					}
					//
					for(int ButtonIndex = 0; 
						ButtonIndex < ArrayCount(Input->Keyboard.Buttons);
						++ButtonIndex){
						Input->Keyboard.Buttons[ButtonIndex].HalfTransitionCount=0;
					}
					// windows messages
					win32_ProcessPendingMessages(Input);
					//
					POINT MouseP;
					GetCursorPos(&MouseP);
					ScreenToClient(WindowHandle, &MouseP);
					Input->Mouse.X = MouseP.x;
					Input->Mouse.Y = MouseP.y;
					Input->Mouse.Z = 0;
					win32_ProcessKeyMessage(&Input->Mouse.Buttons[0],
						GetKeyState(VK_LBUTTON) & (1 << 15));
					win32_ProcessKeyMessage(&Input->Mouse.Buttons[1],
						GetKeyState(VK_MBUTTON) & (1 << 15));
					win32_ProcessKeyMessage(&Input->Mouse.Buttons[2],
						GetKeyState(VK_RBUTTON) & (1 << 15));
					win32_ProcessKeyMessage(&Input->Mouse.Buttons[3],
						GetKeyState(VK_XBUTTON1) & (1 << 15));
					win32_ProcessKeyMessage(&Input->Mouse.Buttons[4],
						GetKeyState(VK_XBUTTON2) & (1 << 15));
					//
					thread_context Thread = {};
					offscreen_buffer Buffer = {};
					Buffer.Memory = GlobalBackbuffer.Memory;
					Buffer.Width = GlobalBackbuffer.Width;
					Buffer.Height = GlobalBackbuffer.Height;
					Buffer.Pitch = GlobalBackbuffer.Pitch;
					if(MarcyCode.UpdateAndRender){
						MarcyCode.UpdateAndRender(&Thread, &Memory, Input, &Buffer);
					}
					//
					LARGE_INTEGER WorkCounter = win32_GetWallClock();
					real32 WorkSecondsElapsed = win32_GetSecondsElapsed(LastCounter, 
						WorkCounter);
					//
					real32 SecondsElapsedForFrame = WorkSecondsElapsed;
					if(SecondsElapsedForFrame < TargetSecondsPerFrame){
						if(SleepIsGranular){
							DWORD SleepMS = (DWORD)(1000.0f * 
								(TargetSecondsPerFrame - SecondsElapsedForFrame)-1.5);
							if(SleepMS > 0){
								Sleep(SleepMS);
							}
						}
						real32 TestSecondsElapsedForFrame = 
								win32_GetSecondsElapsed(LastCounter, win32_GetWallClock());
						// Assert(TestSecondsElapsedForFrame <= TargetSecondsPerFrame);
						while(SecondsElapsedForFrame < TargetSecondsPerFrame){
							SecondsElapsedForFrame = 
								win32_GetSecondsElapsed(LastCounter, win32_GetWallClock());
						}
					}else{
						// TODO(doc): missed frame rate (to log)
					}
					//
					LARGE_INTEGER EndCounter = win32_GetWallClock();
					real64 MSPerFrame = (1000.0f *
						win32_GetSecondsElapsed(LastCounter, EndCounter));
					LastCounter = EndCounter;
					//
					window_dimension Dimension = win32_GetWindowDimension(WindowHandle);
					win32_UpdateWindow(DeviceContext, &GlobalBackbuffer, 
										Dimension.Width, Dimension.Height);
					//
					/*int64 EndCycleCount = __rdtsc();
					uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
					LastCycleCount = EndCycleCount;
					
					real64 FPS = 1000.0f * (1.0f / MSPerFrame);
					real64 MCPF = ((real64)CyclesElapsed / (1000.0f * 1000.0f));
					char temp[1024];
					_snprintf_s(temp, sizeof(temp),
						"%.02f ms/f, %.02f f/s, %.02f  Mc/f\n", MSPerFrame, FPS, MCPF);
					OutputDebugStringA(temp);*/
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