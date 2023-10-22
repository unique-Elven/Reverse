#include <stdio.h>//包含头文件
#include<string.h>//包含头文件
#include<ctype.h>//专门专门处理字符串

typedef struct Node
{
	char BookName[50];//书名
	int BookNumber;//书号
	float BookPrice;//定价
	struct Node* next;//指向下一个节点的指针
}Node;
Node* AppendNode(Node* head, char* BookName, float BookPrice, int BookNumber);
void QueryNode(Node* head, char* BookName);
void ModifyNode(Node* head, char* BookName, float BookPrice);
Node* DeleteNode(Node* head, char* BookName);
int main()
{
	Node* head = NULL;//链表头指针
	char szBookName[50];
	float fBookPrice = 0;
	float fNewBookPrice = 0;
	int nBookNumber = 0;
	int Readflag = 0;
	while (1)
	{
		printf("请选择你需要使用的功能：\n");
		printf("1.添加书籍信息\n");
		printf("2.查询书籍信息\n");
		printf("3.修改书籍信息\n");
		printf("4.删除书籍信息\n");
		scanf("%d", &Readflag);
		switch (Readflag)
		{
		case 1:
			printf("请输入书名：");
			scanf("%s", szBookName);
			printf("数入定价：");
			scanf("%f", &fBookPrice);
			printf("输入书号：");
			scanf("%d", &nBookNumber);
			head = AppendNode(head, szBookName, fBookPrice, nBookNumber);
			break;
		case 2:
			printf("请输入书名：");
			scanf("%s", szBookName);
			QueryNode(head, szBookName);
			break;
		case 3:
			printf("请输入书名：");
			scanf("%s", szBookName);
			printf("请输入新定价：");
			scanf("%f", &fNewBookPrice);
			ModifyNode(head, szBookName, fNewBookPrice);
			break;
		case 4:
			printf("请输入书名：");
			scanf("%s", szBookName);
			head = DeleteNode(head, szBookName);
			break;
		default:
			break;
		}

	}
	return 0;
}

Node* AppendNode(Node* head, char* BookName, float BookPrice, int BookNumber)//末尾添加节点
{
	Node* pNewNode = NULL;//生成一个新节点指针
	Node* pHeadNode = head;//生成一个指向头指针的指针
	pNewNode = (Node*)malloc(sizeof(Node));//申请一个新的节点并申请内存
	if (pNewNode == NULL)//如果没有申请成功
	{
		printf("memory malloc failed!\n");
		exit(0);
	}
	if (head == NULL)//如果没有头节点
	{
		head = pNewNode;//设置一个头节点
	}
	else
	{
		while (pHeadNode->next != NULL)
		{
			pHeadNode = pHeadNode->next;
		}
		pHeadNode->next = pNewNode;
	}
	strcpy(pNewNode->BookName, BookName);
	pNewNode->BookPrice = BookPrice;
	pNewNode->BookNumber = BookNumber;
	pNewNode->next = NULL;
	return head;
}

void QueryNode(Node* head, char* BookName)//查询功能
{
	int flag = 0;
	if (head == NULL)//如果头指针为空
	{
		printf("head == NULL!");
		exit(0);
	}
	if (strcmp(BookName, head->BookName) == 0)
	{
		printf("书名：%s\n定价：%f\n书号：%d\n", head->BookName, head->BookPrice, head->BookNumber);
		flag = 1;
	}
	while (head->next != NULL)
	{
		head = head->next;
		if (strcmp(BookName, head->BookName) == 0)
		{
			printf("书名：%s\n定价：%f\n书号：%d\n", head->BookName, head->BookPrice, head->BookNumber);
			flag = 1;
		}
	}
	if (flag == 0)
	{
		printf("Query failed!");
	}
}

void ModifyNode(Node* head, char* BookName, float BookPrice)
{
	int flag = 0;
	if (head == NULL)//如果头指针为空
	{
		printf("head == NULL!");
		exit(0);
	}
	if (strcmp(BookName, head->BookName) == 0)
	{
		head->BookPrice = BookPrice;
		printf("新定价：%f\n", head->BookName, head->BookPrice, head->BookNumber);
		flag = 1;
	}
	while (head->next != NULL)
	{
		head = head->next;
		if (strcmp(BookName, head->BookName) == 0)
		{
			printf("新定价：%f\n", head->BookName, head->BookPrice, head->BookNumber);
			flag = 1;
		}
	}
	if (flag == 0)
	{
		printf("Modify failed!");
	}
}

Node* DeleteNode(Node* head, char* BookName)
{
	Node* pNode = NULL;
	if (head == NULL)//如果头指针为空
	{
		printf("head == NULL!");
		exit(0);
	}
	if (strcmp(BookName, head->BookName) == 0)
	{
		if (head->next != NULL)
		{
			pNode = head->next;
			free(head);
			return pNode;
		}
		else
		{
			printf("List NNLL!");
			return NULL;
		}
	}
	if (strcmp(BookName, head->next->BookName) == 0);
	{
		if (head->next->next != NULL)
		{
			pNode = head->next->next;
			head->next = head;
			return head;
		}
	}
	while (head->next->next != NULL)
	{
		if (strcmp(BookName, head->next->next->BookName) == 0)
		{
			pNode = head->next->next->next;
			head->next->next = pNode;
			return head;
		}
	}
}
