/**
 *
 *  TODO:增加限制输入模块，现在可以长按j而持续上升，这在真实游戏中是不可以的，
 *
 * TOFO:加入计分模块
 *
 * TODO：修改回收模块
 */

#define HAVEBAR 5
#define bool int
#define TRUE 1
#define FALSE 0
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

//柱子结构体
struct stBAR
{						//所有不被使用的柱子的位置都在画面外
	int isUsing;
	int y;
	int width;		//柱子的宽度
	int h1, h2;		//两个高度
	int distant;	//与下一个柱子的距离，即此柱子之右与下一个柱子左的距离
};

//全局变量
int bird_x, bird_y;				//鸟的坐标
double bird_v;					//鸟的速度
int high, width;				//游戏窗口尺寸
const double spaceVolocity = -3;//按空格之后鸟的速度
const long double g = 0.00001;			//重力加速度
double t0, t1, timeDeerta;		//小鸟自由落体用，见文件末尾图示
int isRising;					//记录是否正在上升
char input;						//输入的容器
struct stBAR bar[HAVEBAR];			//障碍物结构体

//函数声明
void gotoxy(int x, int y);
void startup();
void show();
void updateWithoutInput();
void updateWithInput();
bool isGameOver();
void gameOver();
void updateBarWithoutInput();		//从updateWithoutInput中脱离的关于障碍物的部分
int getRandInt(int min, int max);	//一个随机数，大小介于输入值

//主函数
int main(int argc, char const *argv[])
{
	startup();
	while(1)
	{
		show();
		updateWithoutInput();
		updateWithInput();
		if (isGameOver())
		{
			break;
		}
	}
	gameOver();
	return 0;
}

void gotoxy(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(handle,pos);
}

void startup()
{
	high = 35;
	width = 50;
	bird_x = 1;
	bird_y = width/7;
	bird_v = 0;
	for (int i = 0; i < HAVEBAR; ++i)
	{
		bar[i].isUsing = FALSE;		//所有柱子都没有在使用中
		bar[i].y = width+1;			//所有柱子都放在外面
	}
}

void show()
{
	gotoxy(0,0);
	int i, j;

	//测试专用
	{
		printf("bird:(%2d,%2d)\n", bird_x, bird_y);

		//正在使用的柱子
		int minY = -1;
		int front = HAVEBAR+2;
		for (int i = 0; i < HAVEBAR; ++i)
		{
			
			if (bar[i].isUsing)
			{
				printf("1");
				if (bar[i].y < minY)
				{
					minY = bar[i].y;
					front = i;
				}
			}
			printf("0");
		}
		printf("\n");
		printf("front:%d\n", front);
	}

	for (i = 0; i < high; ++i)		//列
	{
		for (j = 0; j < width; ++j)	//行
		{
			if (i == bird_x && j == bird_y)
			{
				printf("@");		//鸟
				goto haveDrewBar;
			}

			//通过循环找出障碍物的位置
			for (int k = 0; k < HAVEBAR; ++k)
			{
				//	高度		  &&	左面的右	&&		右面的左
				if (i < bar[k].h1 && j >= bar[k].y && j <= bar[k].y+bar[k].width)		//上柱
				{
					printf("+");goto haveDrewBar;
				}
				if (i > bar[k].h2 && j >= bar[k].y && j <= bar[k].y+bar[k].width)		//下柱
				{
					printf("+");goto haveDrewBar;
				}

				//测试专用
				/*{
					if (j == bar[k].y)		//左面
					{
						printf("k");goto haveDrewBar;
					}		
					if (j == bar[k].y+1)	//左面第二
					{
						printf("-");goto haveDrewBar;
					}
					if (j == bar[k].y+bar[k].width-1)		//右面
					{
						printf("j");goto haveDrewBar;
					}								
				}*/
			}
			
			if (i == 0 || j == 0 || i == high-1 || j == width-1)
			{
				printf(".");		//边界
			}
			else
			{
				printf(" ");
			}
			haveDrewBar:
			1==1;
		}

			printf("\n");			//换行
	}
}

void updateBarWithoutInput()
{
	//生成柱子				
								/*struct stBAR
								{						//所有不被使用的柱子的位置都在画面外
									int isUsing;
									int y;
									int h1, h2;		//两个高度
									int distant;	//与下一个柱子的距离
								};*/
	static int theNbOfUsingBar;			//正在使用的柱子的数量
	theNbOfUsingBar = 0;	//重置数据，下面要统计
	for (int i = 0; i < HAVEBAR; ++i)//对于所有柱子
	{
		//统计正在使用的柱子
		if (bar[i].isUsing)
		{
			theNbOfUsingBar++;
		}
		//如果没有柱子在使用，则生成一个柱子
		if (theNbOfUsingBar == 0)
		{
			bar[0].isUsing = TRUE;
			bar[0].y = width;
			bar[0].width = getRandInt(10, 15);
			bar[0].h1 = getRandInt(high/2-10,high/2-5);
			bar[0].h2 = getRandInt(high/2+5,high/2+10);
			bar[0].distant = getRandInt(5, 10);
		}
		//定义前一个号码
		int last = (i+1)%HAVEBAR;		//普通的前一个号码	//如果是初号柱子，则号码为最后一个


		//如果到了前一个柱子适当距离，而且本柱子没有使用，则生成
		if (bar[last].y <= width-bar[last].distant-bar[last].width && !bar[i].isUsing)
		{

			bar[i].isUsing = TRUE;					//是否使用中
			bar[i].y = width;						//位置
			bar[i].width = getRandInt(5, 10);		//赋予障碍物随机宽度
			bar[i].h1 = getRandInt(10, 15);				//上柱位置
			bar[i].h2 = getRandInt(high-20, high-15);	//下柱位置
			bar[i].distant = bar[i].widh;			//与下一障碍物的距离
		}
	}

	//移动柱子
	for (int i = 0; i < HAVEBAR; ++i)
	{
		if (bar[i].isUsing)
		{
			bar[i].y--;
		}
	}	
	//如果柱子离开画面，则回收柱子
	for (int i = 0; i < HAVEBAR; ++i)
	{
		if (bar[i].y + bar[i].width< 0)
		{
			bar[i].isUsing = FALSE;
		}
	}		
}

void updateWithoutInput()
{
	static int speedLimit = 10;
	static int gameSpeed = 0;	
	static int isTheFirsttime = 1;


	//测试用
	{
		printf("kbhit:%d\n", kbhit());
		printf("isRising:%d\n", isRising);
	}

   	//得出时间差
	if (isTheFirsttime)
	{
		t0 = clock();
		isTheFirsttime = 0;
	}
	else
	{
		t1 = clock();
		timeDeerta = t1 - t0;
	}

	
	//移动小鸟
		if (isRising >= 0)
	{
		bird_x--;		//上升
		isRising -= 1;
	}

	gameSpeed++;
	if (gameSpeed >= speedLimit)
	{
		
			//按照时间差调节speedLimit
				if (timeDeerta < 500)
				{
					speedLimit = 3;
				}
				else if (timeDeerta <= 800)
				{
					speedLimit = 2;
				}
				else if (timeDeerta <= 1000)
				{
					speedLimit = 1;
				}
				else
				{
					speedLimit = 0;
				}				
			bird_x++;		//下降
		
			gameSpeed = 0;
	}
	updateBarWithoutInput();
}

void updateWithInput()
{
	//TODO:t0
	if (kbhit())			//如果输入为空格，则上升
	{
		input = getch();
		if (input =='j')
		{
			t0 = clock();
			isRising = 5;
		}
		else if (input == 'p')	 //暂停
		{
			while(1)
			{	
				input = getch();
				if (input == 'p')
				{
					break;
				}
			}
		}
			
	}
}
bool isGameOver()		//判断游戏是否结束
{
	//判断是否落地
	bool theAnswer = FALSE;
	if (bird_x >= high)
	{
		theAnswer = TRUE;
	}
	//判断是否碰撞
	for (int i = 0; i < HAVEBAR; ++i)
	{
		//这里的条件的先后可以微小地影响流畅度，有空再重构

		//	正在使用 && 左的右边 && 右的左边
		if (bar[i].isUsing == TRUE && bird_y >= bar[i].y && bird_y <= bar[i].y+bar[i].width)
		{	//	下柱||上柱 
			if (bird_x < bar[i].h1 || bird_x > bar[i].h2)
			{
				theAnswer = TRUE;
			}
		}
	}	
	
	return theAnswer;
}
void gameOver()
{
	//TODO
}

/**
 *下面是时间轴
 *
 *
 *   		   .
 ^           .
 * 		   .
 *       . 
 *     .
 V   .
 * .
 *
 *  ____timeDeerta______
 * |                   |
 ******************************************************>
 * t0                 t1
 *
 * t0：			按下空格或者开始时重置
 * t1：			每次运算的时间点
 * timeDeerta:	时间差，用于运算
 *
 */
int getRandInt(int min, int max)
{
	if (max > min)
	{
		int i;
		srand(clock());
		i = (rand()%(max-min)) + min;
		return i;
	}
	else
	{
		return 0;
	}

}