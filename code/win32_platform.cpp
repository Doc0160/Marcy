#include <windows.h>
#include <stdint.h>

#include "central_piece.cpp"

#define bool32 int32_t

#define internal static
#define global static

global bool global_running=0;
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
	WindowClass.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance   = Instance;
	// WindowClass.hIcon         = ;
	WindowClass.lpszClassName = "MARCY_WC";
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
			while(global_running){
				// windows messages
				MSG Message;
				while(PeekMessageA(&Message,0,0,0,PM_REMOVE)){
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
			}
		}
	}
}