#include <iostream>
#include <windows.h>

int gTotal = 0;
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{

	while (true)
	{
		int address = 0x00411000;
		int byteToal = 0;
		for (int Index = 0; Index < 0x6000; Index++)
		{
			byte byteRead = 0;
			ReadProcessMemory((HANDLE)-1, (LPVOID)address, &byteRead, 1, NULL);
			byteToal += byteRead;
			address++;
		}
		if (byteToal != gTotal)
		{
			printf("检测到更改代码！程序已经退出！！！");
			exit(0);
		}
	}
	return TRUE;
}
int main()
{
	int i = 0;
	int address = 0x00411000;
	for (int Index = 0; Index < 0x6000; Index++)
	{
		byte byteRead = 0;
		ReadProcessMemory((HANDLE)-1, (LPVOID)address, &byteRead, 1, NULL);
		gTotal += byteRead;
		address++;
	}
	CreateThread(NULL, NULL, ThreadProc, NULL, NULL, NULL);
	while (1)
	{
		Sleep(1500);
		printf("%d 正常执行中....\n",i);
		i++;
	}
	system("pause");
	return 0;
	
}