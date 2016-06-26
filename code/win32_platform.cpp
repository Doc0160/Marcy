#include "platform.h"
#include <stdint.h>
//
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
//
typedef int32 bool32;
//
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
//
typedef float real32;
typedef double real64;

#include "central_piece.cpp"

#define DINFO(msg) OutputDebugStringA("[ ] "msg"\n");
#define DFAIL(msg) OutputDebugStringA("[-] "msg"\n");
#define DSUCCESS(msg) OutputDebugStringA("[+] "msg"\n");

#define internal static
#define global static

#include <windows.h>

#define Assert(x) \
	do { if (!(x)) { __debugbreak(); } } while(0)

void Debugf(char *format, ...){
	char temp[1024];
	va_list arg_list;
	va_start(arg_list, format);
	wvsprintfA(temp, format, arg_list);
	va_end(arg_list);
	OutputDebugStringA(temp);
}

#undef OutputDebugString
#undef RegisterClass
#undef DefWindowProc
#undef CreateWindowEx
#undef GetMessage
#undef DispatchMessage
#undef WNDCLASS
#undef MessageBox
#undef LoadLibrary

struct window_dimension{
	int Width;
	int Height;
};
struct win32_offscreen_buffer{
	// NOTE(doc): pixels are alwasy 32-bits wide, memory order BB GG RR XX
	BITMAPINFO Info;
	void       *Memory;
	int        Width;
	int        Height;
	int        Pitch;
};
//
global bool32 global_running = 0;
global win32_offscreen_buffer GlobalBackbuffer = {};
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
	Backbuffer->Memory = VirtualAlloc(0,BitmapMemorySize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
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
			global_running=0;
		}break;
		case WM_PAINT:{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(wnd, &Paint);
			window_dimension Dimension = win32_GetWindowDimension(wnd);
			win32_UpdateWindow(DeviceContext, &GlobalBackbuffer,
				Dimension.Width, Dimension.Height);
			EndPaint(wnd, &Paint);
		} break;
		default:{
			// NOTE(doc): let window do his things 'cause some events need specific return values
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
	WNDCLASSA WindowClass = {};
	win32_ResizeDIBSelection(&GlobalBackbuffer, 1280/2, 720/2);
	WindowClass.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance   = Instance;
	// WindowClass.hIcon         = ;
	WindowClass.lpszClassName = "MARCY_WC";
	//
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
	//
	if(RegisterClassA(&WindowClass)){
		HWND WindowHandle = CreateWindowExA(
			0,
			"MARCY_WC",
			"Marcy",
			WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0);
		if(WindowHandle){
			global_running=1;
			HDC DeviceContext = GetDC(WindowHandle);
			//
			offscreen_buffer Buffer = {};
			Buffer.Memory = GlobalBackbuffer.Memory;
			Buffer.Width = GlobalBackbuffer.Width;
			Buffer.Height = GlobalBackbuffer.Height;
			Buffer.Pitch = GlobalBackbuffer.Pitch;
			//
			LARGE_INTEGER LastCounter;
			QueryPerformanceCounter(&LastCounter);
			//
			while(global_running){
				// windows messages
				MSG Message;
				while(PeekMessageA(&Message,0,0,0,PM_REMOVE)){
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				UpdateAndRender(&Buffer);
				//
				window_dimension Dimension = win32_GetWindowDimension(WindowHandle);
				win32_UpdateWindow(DeviceContext, &GlobalBackbuffer, Dimension.Width, Dimension.Height);
				//
				LARGE_INTEGER EndCounter;
				QueryPerformanceCounter(&EndCounter);
				//
				int64 CounterElapsed = EndCounter.QuadPart-LastCounter.QuadPart;
				int32 MSPerFrame = (int32)((1000 * CounterElapsed) / PerfCountFrequency);
				int32 FPS = PerfCountFrequency / CounterElapsed;
				Debugf("%dmspf / %dfps\n", MSPerFrame, FPS);
				//
				LastCounter=EndCounter;
			}
			// TODO(doc): maybe delete that, windows do it anyway ~~
			ReleaseDC(WindowHandle, DeviceContext);
		}
	}
}