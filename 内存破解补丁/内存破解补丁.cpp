// 内存破解补丁.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <iostream>

int main()
{
    int pId;
    printf("请输入要破解的程序进程ID：");
    scanf("%d", &pId);
    byte buff[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    HANDLE hHandle=OpenProcess(PROCESS_ALL_ACCESS, TRUE, pId);
    WriteProcessMemory(hHandle, (LPVOID)0x004010FD, buff, 6, NULL);
}


