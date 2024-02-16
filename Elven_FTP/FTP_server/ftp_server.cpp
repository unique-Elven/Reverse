#include <stdio.h>
#include "ftp_server.h"
char g_recvBuf[1024];//全局变量自动初始化为0,用来接受客户端发送的消息
int g_fileSize;		//文件大小
char* g_fileBuf; //存储文件内容

int main()
{
	initSocket();

	listenToClient();

	closeSocket();
	return 0;
}

//初始化socket库
bool initSocket()
{
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("WSAStartup faild：%d\n",WSAGetLastError());
		return false;
	}
	return true;
}
//关闭socket库
bool closeSocket()
{
	
	if (0 != WSACleanup())
	{
		printf("WSACleanup faild：%d\n", WSAGetLastError());
		return false;
	}
	return true;
}
//监听客户端连接
void listenToClient()
{
	//创建server socket套接字（在系统内核中分配了一块内存空间）
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serfd)
	{
		printf("soket faild:%d\n", WSAGetLastError());
		return;
	}
	//给socket绑定ip地址和端口号
	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;//必须和创建socket指定的一样
	serAddr.sin_port = htons(SPORT);//把本地字节序转化成网络字节序
	serAddr.sin_addr.S_un.S_addr = ADDR_ANY;//监听本机所有网卡
	if (0 != bind(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr)))
	{
		printf("bind faild：%d\n", WSAGetLastError());
		return;
	}

	//监听客户端链接
	if (0 != listen(serfd,10))
	{
		printf("listen faild：%d\n", WSAGetLastError());
		return;
	}

	//有客户端链接，那就要接受链接
	struct sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	SOCKET clifd =  accept(serfd, (struct sockaddr*) & cliAddr, &len);
	if (INVALID_SOCKET == clifd)
	{
		printf("accept faild：%d\n", WSAGetLastError());
		return;
	}

	//开始处理消息
	while (processMsg(clifd))
	{
		Sleep(5000);
	}
}

//处理消息
bool processMsg(SOCKET clifd)
{
	//成功接收消息，返回接收到的字节数，接受关闭返回0，否则SOCKET_ERROR
	int nRes = recv(clifd, g_recvBuf, 1024, 0);
	if (nRes <= 0)
	{
		printf("客户端下线。。。%d\n", WSAGetLastError());
	}
	//获取接收到的消息
	struct MsgHeader* msg = (struct MsgHeader*)g_recvBuf;
	switch (msg->msgID)
	{
	case MSG_FILENAME:
		printf("%s %d\n", msg->fileInfo.fileName, nRes);
		readFile(clifd, msg);
		break;
	case MSG_SENDFILE:
		printf("MSG_SENDFILE\n");
		sendfile(clifd, msg);
		break;
	default:
		break;
	}
	
	return true;
}

bool readFile(SOCKET clifd, MsgHeader* pmsg)
{
	FILE* pread = fopen(pmsg->fileInfo.fileName, "rb");
	if (pread == NULL)
	{
		printf("找不到【%s】文件", pmsg->fileInfo.fileName);
		struct MsgHeader msg = { msg.msgID = MSG_OPENFILE_FALID };
		if (SOCKET_ERROR == send(clifd, (char*)&msg, sizeof(struct MsgHeader),0))
		{
			printf("send faild：%d\n", WSAGetLastError());
		}
		return false;
	}
	//获取文件的大小
	fseek(pread,0,SEEK_END);
	g_fileSize = ftell(pread);
	fseek(pread, 0, SEEK_SET);
	//把文件大小发给客户端
	//D:\asus\桌面\PROJECT\逆向\逆向\170.jpg
	struct MsgHeader msg ={msg.msgID = MSG_FILESIZE,msg.fileInfo.fileSize=g_fileSize};
	char tfnfame[200] = { 0 }, text[100];
	_splitpath(pmsg->fileInfo.fileName, NULL, NULL, tfnfame, text);
	strcat(tfnfame, text);
	strcpy(msg.fileInfo.fileName, tfnfame);
	send(clifd, (char*)&msg, sizeof(struct MsgHeader), 0);
	
	//读文件内容
	g_fileBuf = (char*)calloc(g_fileSize + 1, sizeof(char));
	if (g_fileBuf == NULL)
	{
		printf("堆内存空间申请失败！\n");
		return false;
	}
	fread(g_fileBuf, sizeof(char), g_fileSize,pread);
	g_fileBuf[g_fileSize] = '\0';
	fclose(pread);

	return true;
}

	//发送文件
bool sendfile(SOCKET clifd, MsgHeader* pmsg)
{
	//如果文件的长度大于每个数据包能传送的大小（PACKET_SIZE）那么就分块传输
	for (size_t i = 0; i < g_fileSize; i+= PACKET_SIZE)
	{
		struct MsgHeader msg = { msg.msgID = MSG_READY_READ };//告诉客户端准备接受文件
		msg.packet.nStart = i;
		//包的大小大于文件总数据的大小
		if (i+PACKET_SIZE+1 > g_fileSize)
		{
			msg.packet.nStart = g_fileSize - i;
		}
		else
		{
			msg.packet.nStart = PACKET_SIZE;
		}
		memcpy(msg.packet.buf, g_fileBuf+ msg.packet.nStart, msg.packet.nsize);

		if (SOCKET_ERROR == send(clifd, (char*)&msg, sizeof(struct MsgHeader), 0))
		{
			printf("文件发送失败！%d\n", WSAGetLastError());
		}
	}


	return true;
}