#include <stdio.h>
#include <graphics.h> //easyX
#include "resource.h"
//mciSendString需要
#include <mmsystem.h>
#pragma comment(lib,"WINMM.LIB")

#define SPACE 10 //格子宽高10像素
#define COLS 80 //横向格子数
#define ROWS 80 //纵向格子数
int isContinue = 1;//循环
int map[ROWS][COLS] = { 0 };//描述格子状态
//画一个正方形格子，xy为格子左上角的坐标，flag为状态
void drawRect(int x, int y, int flag);

//画整个界面
void drawMap();

//根据鼠标的输入设置二维数组
void getAndSet();

//更新函数
void update();

//自动初始化模式
void startup();

int main()
{
	//1. 窗口做出来
	initgraph(COLS*SPACE, ROWS*SPACE);

	//2. 输出规则（字符串显示）
	char tips[3][200] = {
		"规则1 ： 如果细胞周围有3个存活的细胞 存活\n",
		"规则2 ： 如果细胞周围有2个存活的细胞，维持不变\n",
		"规则3 ： 其他情况 不存活\n"
	};

//#if 0
	settextcolor(RED);
	for (int i = 0;i<3;i++)
	{
		outtextxy(150, 333, tips[i]);
		Sleep(2000);
		cleardevice();
	}
//#endif
	//3. 制作格子图
	//drawMap();
#if 0
	//自动初始化
	startup();
	drawMap();
#endif
	//4. 鼠标来输入 一边显示 一边鼠标输入（多线程，异步io,io多路复用，多进程）
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)getAndSet,NULL,NULL,NULL);
	while (isContinue)
	{
		drawMap();
	}

	//5. 音乐播放
	mciSendString("open 1.mp3 alias mybgm", NULL, 0, NULL);
	mciSendString("play mybgm repeat", NULL, 0, NULL);

	//6. 根据定制的规则（算法）来演变
	while (1)
	{
		update();
		drawMap();
	}
}

void drawRect(int x, int y, int flag)
{
	//边框的颜色
	setcolor(RGB(167, 167, 167));
	if (1==flag)
	{
		setfillcolor(WHITE);
	} 
	else
	{
		setfillcolor(BLACK);
	}
	fillrectangle(x, y, x + SPACE, y + SPACE);
}

void drawMap()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			//Sleep(1);//显示格子生成
			drawRect(j * SPACE, i * SPACE, map[i][j]);
		}
	}
}

void getAndSet()
{
	MOUSEMSG msg;
	int x, y;
	while (isContinue)
	{
		msg = GetMouseMsg();
		//坐标除以宽高等于下标
		x = msg.x / SPACE;
		y = msg.y / SPACE;

		if (msg.mkLButton) {//鼠标左键点击
			map[y][x] = 1;
			if (y == (ROWS - 1) && x == (COLS - 1))
			{
				isContinue = 0;
			}
		}
		else
		{
			map[y][x] = 0;
		}
	}
}

void update()
{
	int NewCells[ROWS][COLS] = { 0 };//下一帧的细胞情况

	//为了方便处理，不对边界处理
	for (int i = 0; i < ROWS - 1; i++)
	{
		for (int j = 0; j < COLS - 1; j++)
		{
			int NeibourNumber;//统计邻居的个数
			NeibourNumber = map[i - 1][j - 1] + map[i - 1][j] + map[i - 1][j + 1] + map[i][j - 1] + map[i][j + 1] + map[i + 1][j - 1] + map[i + 1][j] + map[i + 1][j + 1];
			if (NeibourNumber == 3)//当周围有3个细胞，当前位置细胞为生
			{
				NewCells[i][j] = 1;
			}
			else if (NeibourNumber == 2)//周围有2个细胞，当前位置细胞不变
			{
				NewCells[i][j] = map[i][j];
			}
			else
			{
				NewCells[i][j] = 0;
			}
		}
	}
	if (map[ROWS][COLS] == NewCells[ROWS][COLS])//如果不再演变就结束了，防止消耗资源
	{

	}
	//将结果赋值回map
	for (int i = 0; i < ROWS-1; i++)
	{
		for (int j = 0; j < COLS-1; j++)
		{
			map[i][j] = NewCells[i][j];
		}
	}

}

void startup()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			map[i][j] = rand() % 2;//细胞随机状态
		}
	}
}
