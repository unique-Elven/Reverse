#include <stdio.h>
#include <windows.h>

int main() 
{
	UCHAR buf[14] = { 0xB9, 0xA7, 0xCF, 0xB2, 0xC4, 0xE3, 0xC6, 0xC6, 0xBD, 0xE2, 0xB3, 0xC9, 0xB9, 0xA6 };
	UCHAR buf1[15] = { 0 };
	for (int i = 0; i < 14; i++)
	{
		buf1[i] = (buf[i]^2) - 5; // str += ×Ö·û([i] + 5)^2)
	}
		

		printf(" %s\n", buf1);


	return 0;
}