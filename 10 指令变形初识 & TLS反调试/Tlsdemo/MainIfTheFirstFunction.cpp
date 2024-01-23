#include <windows.h>
class MyClass
{
public:
	MyClass()
	{
		::MessageBox(NULL, "我是构造函数", "Elven", NULL);
	}
};
MyClass a;
int main()
{
	::MessageBox(NULL, "我是main函数", "Main", NULL);
	return 0;
}