#include <stdio.h>
#include <windows.h>
LONG CALLBACK VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	printf("hello world");
	printf("%08x", pExceptionInfo->ContextRecord->Eip);
	return EXCEPTION_EXECUTE_HANDLER;
}
int main()
{
	AddVectoredContinueHandler(NULL, VectoredHandler);
	__asm int 3;
	system("pause");
}