
#include "resource.h"
#include <Windows.h>
#include <string.h>
#include "Plugin.h"
#pragma comment(lib,"OLLYDBG.LIB")

extern HICON g_hIcon;
LRESULT CALLBACK WndProc( 
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	);
void EDebugStart();
