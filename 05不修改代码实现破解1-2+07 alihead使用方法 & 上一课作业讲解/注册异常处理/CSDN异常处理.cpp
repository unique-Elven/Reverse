#include <iostream>
#include <Windows.h>

LONG MyVEHCallBack(
	struct _EXCEPTION_POINTERS* ExceptionInfo
) {
	::MessageBox(NULL, "DLL", "这只是一个没有用的提示而已", NULL);
	//向量化异常需要我们自己选择处理方式，以及是否回去
	//异常代码
	DWORD dwCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	//判断接收到的是什么异常
	if (dwCode == EXCEPTION_BREAKPOINT) {
		//异常处理
		std::cout << "This is Int3" << std::endl;
		printf("hello world\n");
		printf("%08x\n", ExceptionInfo->ContextRecord->Eip);
		::MessageBox(NULL, "DLL注入成功", "这只是一个没有用的提示而已", NULL);
		system("pause");
		//我们这里想处理异常后，回去，继续执行后面的代码
		//EIP 指令指针寄存器，保存了下一行要执行的位置
		//我们将EIP+1，就会执行后面的代码了
		ExceptionInfo->ContextRecord->Eip += 1;
		//返回，继续执行
		return EXCEPTION_EXECUTE_HANDLER;
	}
	return EXCEPTION_CONTINUE_EXECUTION;
	return 0;
}

int main1()
{
	/*
	//线程上下文获取
	CONTEXT ctx;
	GetThreadContext(GetCurrentThread(), &ctx);
	//设置线程上下文
	ctx.Dr0 = 111;
	SetThreadContext(GetCurrentThread(), &ctx);
	*/

	//注册向量异常处理程序
	AddVectoredExceptionHandler(
		0,      //调用处理程序的顺序，程序第一个处理异常还是最后处理异常
		(PVECTORED_EXCEPTION_HANDLER)MyVEHCallBack   //指向要调用处理程序的指针，实际上就是我们定义的处理异常函数
	);
	int i = 0;
	while (true)
	{
		i++;
		Sleep(500);
		if (i == 3)
		{
			_asm {
				int 3
			}
			std::cout << "异常之后的代码" << std::endl;
		}
		printf("%d\n", i);
	}
	//这里我们使用汇编，触发一个int3断点
	//int3 软件断点 0xCC
	/*_asm int 3;*/
	

	

	system("pause");
	return 0;
}

// ————————————————
// 版权声明：本文为CSDN博主「Shad0w - 2023」的原创文章，遵循CC 4.0 BY - SA版权协议，转载请附上原文出处链接及本声明。
// 原文链接：https ://blog.csdn.net/qq_73985089/article/details/132384370
