#include "InDialog.h"

HINSTANCE g_hInst;
HWND g_hMain;
HICON g_hIcon;
int WINAPI DllMain(_In_ HANDLE _HDllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID _Reserved)
{
	g_hInst = HINSTANCE(_HDllHandle);
	return 1;
}
extc int _export cdecl ODBG_Plugindata(char shortname[32])
{
	strcpy_s(shortname,11,"E-Debug2.0");
	return PLUGIN_VERSION;
}
extc int _export cdecl ODBG_Plugininit(int ollydbgversion, HWND hw, DWORD *features)
{
	if (ollydbgversion < PLUGIN_VERSION)
	{
		MessageBoxA(hw,"插件与OD版本不匹配!","Xjun提示：",MB_ICONERROR);
		return -1;
	}
	g_hMain = hw;

	_Addtolist(0,0,"%s","E-Debug 2.0");
	_Addtolist(0,-1,"%s","   Coded By:Xjun");
	return 0;
}
extc int _export cdecl ODBG_Pluginmenu(int origin, TCHAR data[4096], VOID *item)
{
	if (origin == PM_MAIN)
	{
		strcpy_s(data,19,"0&Analysis|1&About");

	}
	return 1;
}
extc void _export cdecl ODBG_Pluginaction(int origin, int action, VOID *item)
{
	if (origin == PM_MAIN)
	{
		if (action == 0)
		{
			g_hIcon = LoadIconA(g_hInst,LPCSTR(IDI_ICON1));
			DialogBoxParamA(g_hInst,LPCSTR(IDD_DIALOG1),NULL,(DLGPROC)WndProc,NULL);
		}
		else if (action == 1)
		{
			MessageBoxA(g_hMain,"E-Debug2.0 \r\nCoded By：Xjun","About",MB_ICONINFORMATION);
		}
	}
}