#include <stdio.h>
#include <string.h>
#include <windows.h>
// int __cdecl keygen(char* Str, char* Str1, unsigned int a3)
// {
// 	signed int v4; // [esp+0h] [ebp-Ch]
// 	size_t j; // [esp+4h] [ebp-8h]
// 	signed int i; // [esp+8h] [ebp-4h]
// 
// 	__CheckForDebuggerJustMyCode(byte_45A007);
// 	if (!Str)
// 		return -1;
// 	if (!Str1 || !a3)
// 		return -1;
// 	if (strlen(Str) <= a3)
// 		v4 = strlen(Str);
// 	else
// 		v4 = a3;
// 	for (i = 0; i < v4; ++i)
// 	{
// 		for (j = 0; j < strlen("bcdaren"); ++j)
// 			Str1[i] = byte_442198[j] ^ (Str[i] + 13);
// 	}
// 	return 0;
// }
//Str1=((++**--,,//..QQPP
int keygen(char *Str,char *flag)
{
	char byte_442198[8] = "bcdaren";
	int a2 = 136;
	for (int i = 0; i < 18; ++i)
	{
		for (int j = 0; j < strlen(byte_442198); ++j)
			flag[i] = (byte_442198[j] ^ Str[i])-13;
			flag[i + 1] = '\n';
	}
	return *flag;
}
int main()
{
	char String[] = "((++**--,,//..QQPP";
	char flag[128] = { 0 };
	*flag = keygen(String, flag);
	printf("%s", &flag);
	system("pause");
}