#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include "LPC/ntdll.h"

//_IMAGE_TLS_DIRECTORY32
//我要用到TLS这个东西了
#pragma comment(linker,"/INCLUDE:__tls_used")

DWORD isDebug = 0;
void NTAPI TLS_CALLBACK(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
	//::MessageBox(NULL, "TLS函数执行1", "Elven", NULL);
	if (Reason == DLL_PROCESS_ATTACH)
	{
		::MessageBox(NULL, "TLS函数执行1", "Elven", NULL);
		//不接受内核调试信息 第一个参数是进程句柄，第二个参数是想干啥(我想中断和内核之间调试信息的联系)
		//NtSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, 0, 0);

		//返回值isDebug为0则没被调试，如果不为0证明被调试了
		NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort, (PVOID)&isDebug, sizeof(DWORD), NULL);
	}
}
void NTAPI TLS_CALLBACK2(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		::MessageBox(NULL, "TLS函数执行2", "Elven", NULL);
		//不接受内核调试信息 第一个参数是进程句柄，第二个参数是想干啥(我想中断和内核之间调试信息的联系)
		//NtSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, 0, 0);

		//返回值isDebug为0则没被调试，如果不为0证明被调试了
		NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort, (PVOID)&isDebug, sizeof(DWORD), NULL);
	}
}
int main(void)
{
// 	char array[] = { 0xD7,0xA2,0xB2,0xE1,0xB3,0xC9,0xB9,0xA6,0x00 };//注册成功
// 	for (int i = 0; i < 9; i++)
// 	{
// 		array[i] = array[i] ^ 0x10;
// 	}
	char array2[] = { 0xc7, 0xb2, 0xa2, 0xf1, 0xa3, 0xd9, 0xa9 ,0xb6 ,0x10};
	for (int i = 0; i < 9; i++)
	{
		array2[i] = array2[i] ^ 0x10^isDebug;//由于如果是0那么异或任何数都为原来的数，利用这个特性实现反调试，因为调试了isDebug会变，结果也就会变.
		//但是画眉的OD有反反调试插件，所以可能没变要注意。
	}
	//0xc7, 0xb2, 0xa2, 0xf1, 0xa3, 0xd9, 0xa9 ,0xb6 ,0x10
	MessageBox(NULL, array2, "Elven", MB_OK);
	system("pause");
	return 0;
}

//新建一段数据，放到TLS这个目录表里面
#pragma data_seg(".CRT$XLX")
PIMAGE_TLS_CALLBACK pTLS_CALLBACKs[] = { TLS_CALLBACK,TLS_CALLBACK2,NULL };
#pragma data_seg()