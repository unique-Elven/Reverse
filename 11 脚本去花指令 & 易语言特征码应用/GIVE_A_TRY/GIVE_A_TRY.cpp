#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <random>
// 马勒个蛋的全局数组
int dword_4030B4[42] = {
	0x63B25AF1, 0xC5659BA5, 0x4C7A3C33, 0x0E4E4267, 0xB6117698, 0x3DE6438C, 0x84DBA61F, 0xA97497E6, 0x650F0FB3,
	0x84EB507C, 0xD38CD24C, 0xE7B912E0, 0x7976CD4F, 0x84100010, 0x7FD66745, 0x711D4DBF, 0x5402A7E5, 0xA3334351,
	0x1EE41BF8, 0x22822EBE, 0xDF5CEE48, 0xA8180D59, 0x1576DEDC, 0xFCD62B3B, 0x32AC1F6E, 0x9364A640, 0xC282DD35,
	0x14C5FC2E, 0xA765E438, 0x7FCF345A, 0x59032BAD, 0x9A5600BE, 0x5F472DC5, 0x5DDE0D84, 0x8DF94ED5, 0xBDF826A6,
	0x515A737A, 0x4248589E, 0x38A96C20, 0xCC7F6109, 0x2638C417, 0xD9BEB996
};
int loc_401166(int random, int i) {
	__asm{
		mov     eax, random
		mov     ecx,i
		mul     ecx
		mov     ecx, 0xFAC96621
		pop     eax
		push    edx
		mul     eax
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		mul     edx
		div     ecx
		mov     eax, edx
		pop     edx
		mul     edx
		div     ecx
// 		cmp     edx, [edi + ebx * 4]
// 		jnz     short loc_4011F2
		mov     eax,edx
	}
}
bool isBoomShakaLaka(int result, int i) 
{
	return result == dword_4030B4[i] ? true : false;
}
//爆破随机数
int bruteforceRand()
{
	int dword_46406C = 0x3133359;
	for (size_t i = 0; i < 0xFFFFFFFF; i++)
	{
		int boom = loc_401166(i, 'f');//第一个字符是f
		if (isBoomShakaLaka(boom, 0))
		{
			printf("BoomShakaLakal BoomShakaLaka! \n\rThe first fucking rand is: 0x%x \n\r",i);
			return i;
		}
	}
		
}

//爆破随机种子
int bruteforceSrand(int first)
{
	int dword_40406C = 0x31333359;//过了反调试这个值才是正确的
	for (size_t i = 0; i < 0xFFFFFFFF; i++)
	{
			//因为是伪随机数，种子对上了就能算出一样的随机数
			int random = dword_40406C + i;
			srand(random);
			if (rand() == first)//破解出来的第一次的随机数
			{
				printf("captain! Your fucking seed is: 0x%0x\n\r", random);
				return random;
			}
	}		
}

int main(){
	// 结果是0x4077
	int rnd = bruteforceRand();
	//结果是0x31333d38
	int seed = bruteforceSrand(rnd);
	int possibles = 0xFF * 42;//全部可能
	srand(seed);
	printf("Givmme answer plz: ");
	//马勒个蛋的暴力破解
	for (size_t i = 0; i < 42; i++)
	{
		int nextRnd = rand();//所有的字符可能
		for (size_t j = 1; j < 0xFF; j++)
		{
		
			int fuck = loc_401166(nextRnd,j);
			if (isBoomShakaLaka(fuck,i))
			{
				printf("%c",j);
				break;
			}
		}
	}
	
	printf("\r\n");
	system("pause"); 
	return 0;
}