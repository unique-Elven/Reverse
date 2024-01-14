#include <iostream>
#include <windows.h>

int gTotal = 0;
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{

	while (true)
	{
<<<<<<< HEAD
		int address = 0x00411000;
		int byteToal = 0;
		for (int Index = 0; Index < 0x6000; Index++)
=======
		int address = 0x00401000;
		int byteToal = 0;
		for (int Index = 0; Index < 0x3000; Index++)
>>>>>>> 25fcf6729ef2afae0f0307f71ffca12fc6281b56
		{
			byte byteRead = 0;
			ReadProcessMemory((HANDLE)-1, (LPVOID)address, &byteRead, 1, NULL);
			byteToal += byteRead;
			address++;
		}
		if (byteToal != gTotal)
		{
<<<<<<< HEAD
			printf("检测到更改代码！程序已经退出！！！");
=======
>>>>>>> 25fcf6729ef2afae0f0307f71ffca12fc6281b56
			exit(0);
		}
	}
	return TRUE;
}
int main()
{
<<<<<<< HEAD
	int i = 0;
	int address = 0x00411000;
	for (int Index = 0; Index < 0x6000; Index++)
=======
	int address = 0x00401000;
	for (int Index = 0; Index < 0x3000; Index++)
>>>>>>> 25fcf6729ef2afae0f0307f71ffca12fc6281b56
	{
		byte byteRead = 0;
		ReadProcessMemory((HANDLE)-1, (LPVOID)address, &byteRead, 1, NULL);
		gTotal += byteRead;
		address++;
	}
	CreateThread(NULL, NULL, ThreadProc, NULL, NULL, NULL);
<<<<<<< HEAD
	while (1)
	{
		Sleep(1500);
		printf("%d 正常执行中....\n",i);
		i++;
	}
=======
>>>>>>> 25fcf6729ef2afae0f0307f71ffca12fc6281b56
	system("pause");
	return 0;
	
}