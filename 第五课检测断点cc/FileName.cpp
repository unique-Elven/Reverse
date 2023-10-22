#include <iostream>
#include <windows.h>

int gTotal = 0;
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{

	while (true)
	{
		int address = 0x00401000;
		int byteToal = 0;
		for (int Index = 0; Index < 0x3000; Index++)
		{
			byte byteRead = 0;
			ReadProcessMemory((HANDLE)-1, (LPVOID)address, &byteRead, 1, NULL);
			byteToal += byteRead;
			address++;
		}
		if (byteToal != gTotal)
		{
			exit(0);
		}
	}
	return TRUE;
}
int main()
{
	int address = 0x00401000;
	for (int Index = 0; Index < 0x3000; Index++)
	{
		byte byteRead = 0;
		ReadProcessMemory((HANDLE)-1, (LPVOID)address, &byteRead, 1, NULL);
		gTotal += byteRead;
		address++;
	}
	CreateThread(NULL, NULL, ThreadProc, NULL, NULL, NULL);
	system("pause");
	return 0;
	
}