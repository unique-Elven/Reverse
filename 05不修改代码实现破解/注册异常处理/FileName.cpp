#include <stdio.h>
#include <windows.h>
LONG CALLBACK VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	printf("hello world");
	printf("%08x", pExceptionInfo->ContextRecord->Eip);
	return EXCEPTION_EXECUTE_HANDLER;
}
<<<<<<< HEAD
int main()
=======
int mn()
>>>>>>> 25fcf6729ef2afae0f0307f71ffca12fc6281b56
{
	AddVectoredContinueHandler(NULL, VectoredHandler);
	__asm int 3;
	system("pause");
}