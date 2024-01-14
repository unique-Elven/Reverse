#include <iostream>
#include <Windows.h>
char c='0';
LONG  CALLBACK VectoredHandler(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	
	::MessageBox(NULL, "DLL注入成功", "这只是一个没有用的提示而已", NULL);
	printf("异常地址：%08x\t", ExceptionInfo->ContextRecord->Eip);
	//向量化异常需要我们自己选择处理方式，以及是否回去
	//异常代码
	DWORD dwCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	//判断接收到的是什么异常
	if (dwCode == EXCEPTION_BREAKPOINT) {
		//异常处理
		printf("This is Int3\n");
		
		//我们这里想处理异常后，回去，继续执行后面的代码
		//EIP 指令指针寄存器，保存了下一行要执行的位置
		//我们将EIP+1，就会执行后面的代码了
		printf("请选择继续执行y，还是终止程序n：");
		scanf("%s", &c);
		if (c=='y')
		{
			ExceptionInfo->ContextRecord->Eip += 1;
			//返回，继续执行
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		else if (c=='n')
		{
			system("pause");
			return EXCEPTION_EXECUTE_HANDLER;
		}
		else
		{
			printf("输入错误！");
			return EXCEPTION_EXECUTE_HANDLER;
		}
		
	}
	return EXCEPTION_CONTINUE_EXECUTION;
}
int main()
{
	AddVectoredExceptionHandler(0, (PVECTORED_EXCEPTION_HANDLER)VectoredHandler);
	
	int i = 0;
	while (true)
	{
		i++;
		Sleep(500);
		if (i==3)
		{
			_asm {
				int 3
			}
			std::cout << "异常之后继续执行的代码" << std::endl;
		}
		printf("%d\n", i);
	}
/*	__asm int 3;*/
	system("pause");
	return 0;
}