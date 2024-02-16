#include <stdio.h>
#include "ftp_client.h"

char g_recvBuf[1024] = { 0 };
char* g_fileBuf; //存储文件内容
int g_fileSize;  //文件总大小
char g_fileName[256];	//保存服务器发送过来的文件名
int main()
{
	initSocket();

	connectToHost();

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
void connectToHost()
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
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//连接服务器的ip地址

	//连接到服务器
	
	if (0 != connect(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr)))
	{
		printf("connect faild:%d\n", WSAGetLastError());
		return;
	}

	//开始处理消息
	while (processMsg(serfd))
	{
		Sleep(5000);
	}
}

//处理消息
bool processMsg(SOCKET serfd)
{
	dowloadFileName(serfd);
	
	recv(serfd, g_recvBuf, 1024, 0);
	struct MsgHeader* msg = (struct MsgHeader*)g_recvBuf;
	switch (msg->msgID)
	{
	case MSG_OPENFILE_FALID:
		dowloadFileName(serfd);
		break;
	case MSG_FILESIZE:
		readyread(serfd, msg);
		break;
	case MSG_READY_READ:
		printf("MSG_READY_READ");
		writeFile(serfd, msg);
		break;
	default:
		break;
	}
	return true;
}
void dowloadFileName(SOCKET serfd)
{
	printf("hello!\n");
	char filename[1024] = "你好我是客户端的Elven";
	printf("请输入需要下载的文件：");
	gets_s(filename, 1023);
	struct MsgHeader file ={ file.msgID = MSG_FILENAME};
	strcpy(file.fileInfo.fileName, filename);
	send(serfd, (char *) & file, sizeof(struct MsgHeader), 0);
	//printf("%d %s", file.msgID, file.fileInfo.fileName);
}

void readyread(SOCKET serfd,struct MsgHeader*pmsg)
{
	strcpy(g_fileName , pmsg->fileInfo.fileName);
	g_fileSize = pmsg->fileInfo.fileSize;
	g_fileBuf = (char*)calloc(g_fileSize + 1, sizeof(char));//申请堆内存空间
	if (g_fileBuf == NULL)
	{
		printf("堆内存空间申请失败！\n");
	}
	else
	{
		struct MsgHeader msg = { msg.msgID = MSG_SENDFILE };
		if (SOCKET_ERROR == send(serfd, (char*)&msg, sizeof(struct MsgHeader), 0))
		{
			printf("send faild:%d\n", WSAGetLastError());
			return;
		}
		
	}
	printf("size: %d	filename: %s", pmsg->fileInfo.fileSize, pmsg->fileInfo.fileName);
	//准备内存，pmsg->fileInfo.fileSize
	//准备给服务器发送 MSG_READY_READ
}

bool writeFile(SOCKET serfd, struct MsgHeader* pmsg)
{
	int nStart = pmsg->packet.nStart;
	int nsize = pmsg->packet.nsize;
	memcpy(g_fileBuf,pmsg->packet.buf,pmsg->packet.nsize);
	printf("packet: %d %d\n", nStart + nsize, g_fileSize);
	if (nStart + nsize >= g_fileSize)
	{
		FILE* pwrite = fopen(g_fileName, "wb");
		if (pwrite == NULL)
		{
			printf("write file error。。。\n");
			return false;
		}
		fwrite(g_fileBuf, sizeof(char), g_fileSize, pwrite);
		fclose(pwrite);
		free(g_fileBuf);
		g_fileBuf = NULL;
		struct MsgHeader msg = { msg.msgID = MSG_SUCCESSED };
		send(serfd, (char*)&msg, sizeof(struct MsgHeader), 0);
	}

	return true;
}
