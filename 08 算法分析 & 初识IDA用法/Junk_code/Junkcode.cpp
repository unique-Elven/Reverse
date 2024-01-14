#include <stdio.h>
#include <windows.h>
int __cdecl ChecKey(int userkey)
{
	int result; // eax
	result = userkey * (userkey - 23) == -102;
	if (userkey * (userkey - 23) == -102)
	{
		result = userkey * userkey * userkey;
		if (result == 4913)
			return MessageBoxW(0, L"恭喜你开始了IDA的征程", L"注册成功", 0);
	}
	return result;
}
int main()
{
	__asm
	{
		jmp Label1
		_emit 0xe8
		Label1:
		//注意Label后面一定是冒号，不然会报错！！！！找到“newline"
	}
	__asm
	{
		xor eax, eax
		test eax, eax
		je Label2
		_emit 0e8h
		Label2:
	}

	__asm
	{
		xor eax, eax
		test eax, eax
		je Label3
			jnz label0
			Label0:
				_emit 0e8h//e8是call，这里随便插啥都行
		Label3:
	}
	printf("Test Junk Code");
	for (int userkey = 0; userkey < 0xFFFFFFFF; userkey++)
	{
		if (userkey * (userkey - 23) == -102)
		{
			if (userkey * userkey * userkey == 4913)
			{
				printf("暴力破解：%d", userkey);
			}
		}
	}
	//还能用求根公式解一元二次方程

	system("pause");
}