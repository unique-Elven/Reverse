#include <stdio.h>
#include <string.h>

typedef struct Student
{
	int id=0;
	int age=0;
	char name[30];
};

int main()
{
	Student stu;
	printf("请输入你的学号：");
	scanf("%d", &stu.id);
	printf("请输入你的年龄：");
	scanf("%d", &stu.age);
	strcpy(stu.name, "Elven");
	printf("学号：%d\n", stu.id);
	printf("年龄：%d\n", stu.age);
	printf("名字：%s\n", stu.name);
	return 0;
}