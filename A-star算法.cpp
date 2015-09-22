#include<iostream>
#include<stdio.h>
#define MAXMATRIXSIZE 10
#define MAXSTACKSIZE  100
#define MAXDATA 999
#define QUEUESIZE 100
using namespace std;

// A*算法,斜线可走 

// 问题:1.为什么Move数组中要把直走的方向放前四位,把斜走的放后四位 
//		2.如何用递归而不用队列实现BFS?
//		3.可否用DFS来初始化路径 

int Maze[MAXMATRIXSIZE][MAXMATRIXSIZE] = {
	{1,1,1,1,1,1,1,1,1,1},
	{1,0,0,1,1,1,1,0,0,1},
	{1,1,0,0,0,1,1,1,0,1},
	{1,1,0,1,1,0,1,1,0,1},
	{1,0,0,1,1,0,1,1,0,1},
	{1,1,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,0,1,1,1},
	{1,1,1,1,0,0,0,0,0,1},
	{1,1,0,0,0,1,1,1,0,1},
	{1,1,1,1,1,1,1,1,1,1},
}; 

/*
	f(n) = g(n) + h(n)
(以曼哈顿算法为例):
对于每个固定格子来说,g(n)由父格子确定的,即当前格的g(n) = 父格g(n) + 从父格移动到当前格的路径长度
(每次把一个格子变为其他格子的父格,即选出那个f(n)最小的格子之后,都要计算出它周边的格子的g(n),公式如上 ) 
(而h(n)是一旦确定算法就确定下来的, 不随操作而变,是每个点的固有属性 )
每次选择周围的格子的时候,先找出f(n)最小的格子(称为A),然后计算如果走向格子A,A的g(n)会变成多少
如果A的g(n)反而比它原来的g(n)要大了,就说明走到A格有更好的方法,因此这一步不能走向A
如果新的g(n)<=原来A的g(n) (一般情况下都是=) 那么此时选择走向A格子是正确的 

*/

// (1,1) --> (8,8)

struct Direct{
	int x;
	int y;
};

//Direct Move[8] = { {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0} };	// 从左上开始,下标偶数斜走,奇数直走 
//					  		 上			   	右				下			左 
Direct Move[8] = { {0,-1}, {1,0}, {0,1}, {-1,0}, {-1,-1}, {1,-1}, {1,1}, {-1,1} };	// i<4直走,i>=4斜着走 

struct QueueNode{
	int x;
	int y;
	int Distance;
};

QueueNode Queue[QUEUESIZE];
int front = -1;		//指向队列尾元素的指针 
int rear  = -1;

int g[MAXMATRIXSIZE][MAXMATRIXSIZE];
int h[MAXMATRIXSIZE][MAXMATRIXSIZE];
int Reach = 0;	
int Mark[MAXMATRIXSIZE][MAXMATRIXSIZE];

void AddQ(int x,int y,int Distance){
	rear = (rear+1) % QUEUESIZE;
	Queue[rear].x = x;
	Queue[rear].y = y;
	Queue[rear].Distance = Distance;
	return ;
}

void DeleteQ(int *x,int *y,int *Distance){
	front = (front+1) % QUEUESIZE;			// 队列的数组表示:先移一位再返回! 
	*x = Queue[front].x;
	*y = Queue[front].y;
	*Distance = Queue[front].Distance;
	return ;
}

int isEmpty(){
	return front==rear;
}

void Initialize(){
	//初始化地图上每个点的h(n),因为其只与算法有关
	int i,j;
	memcpy(Mark,Maze,sizeof(Mark));
	memset(g,0,sizeof(g));
	for( i=0; i<MAXMATRIXSIZE; i++){
		for( j=0; j<MAXMATRIXSIZE; j++){
			h[i][j] = MAXDATA;
		}
		cout << endl;
	}
	
	int NextX,NextY,NextDistance;
	int CurX,CurY;
	int Distance = 0; 
	
	CurX = CurY = 8;
	Mark[8][8] = 1 ;					// 把终点标记上 
	AddQ(CurX,CurY,Distance);
	while( !isEmpty() ){
		DeleteQ(&CurX,&CurY,&Distance);
		h[CurX][CurY] = Distance;
		i = 0;
		while( i<8 ){
			NextX = CurX + Move[i].x;
			NextY = CurY + Move[i].y;
			if( !Mark[NextX][NextY] ){
				if( i < 4 ) NextDistance = h[CurX][CurY] + 10;
				else NextDistance = h[CurX][CurY] + 14; 
				Mark[NextX][NextY] = 1;
				AddQ(NextX,NextY,NextDistance);
			}
			i++;
		}
	}
	//test print
	for( i=0; i<MAXMATRIXSIZE; i++){
		for( j=0; j<MAXMATRIXSIZE; j++){
			printf("%4d",h[i][j]);
		}
		cout << endl;
	}
//	getchar();
}

void Astar(int CurX,int CurY){
	printf("%d %d\n",CurX,CurY);
	if( CurX==8 && CurY==8 ){
		Reach = 1;
		return ;
	}
	int NextX,NextY;
	int NextDir = 0;			//下一步的方向(0~3分别四个方向) 
	NextX = CurX;
	NextY = CurY;
	for( int i=0; i<8 && !Reach; i++){
		NextX = CurX + Move[i].x;
		NextY = CurY + Move[i].y;
		g[NextX][NextY] = g[CurX][CurY] + 1;
		if( g[NextX][NextY]+h[NextX][NextY] <= g[CurX][CurY]+h[CurX][CurY] ){
			NextDir = i;
		}
	}
	//找出了f(n)最小的那个,赋值给当前坐标(CurX,CurY) 
	CurX += Move[NextDir].x;
	CurY += Move[NextDir].y;
	Astar(CurX,CurY);
	return ;
}	

void Initialize_Straight(){
	int i,j;
	memcpy(Mark,Maze,sizeof(Mark));
	memset(g,0,sizeof(g));
	for( i=0; i<MAXMATRIXSIZE; i++){
		for( j=0; j<MAXMATRIXSIZE; j++){
			h[i][j] = MAXDATA;
		}
		cout << endl;
	}
	
	int NextX,NextY,NextDistance;
	int CurX,CurY;
	int Distance = 0; 
	
	CurX = CurY = 8;
	Mark[8][8] = 1 ;					// 注意要在之前把终点标记上 
	AddQ(CurX,CurY,Distance);
	while( !isEmpty() ){
		DeleteQ(&CurX,&CurY,&Distance);
		h[CurX][CurY] = Distance;
		i = 0;
		while( i<4 ){
			NextX = CurX + Move[i].x;
			NextY = CurY + Move[i].y;
			if( !Mark[NextX][NextY] ){
				NextDistance = h[CurX][CurY] + 10; 
				Mark[NextX][NextY] = 1;
				AddQ(NextX,NextY,NextDistance);
			}
			i++;
		}
	}
	//test print
	for( i=0; i<MAXMATRIXSIZE; i++){
		for( j=0; j<MAXMATRIXSIZE; j++){
			printf("%4d",h[i][j]);
		}
		cout << endl;
	}
}
	
void Astar_Straight(int CurX,int CurY){
	printf("%d %d\n",CurX,CurY);
	if( CurX==8 && CurY==8 ){
		Reach = 1;
		return ;
	}
	int NextX,NextY;
	int NextDir = 0;			//下一步的方向(0~3分别四个方向) 
	NextX = CurX;
	NextY = CurY;
	for( int i=0; i<4 && !Reach; i++){
		NextX = CurX + Move[i].x;
		NextY = CurY + Move[i].y;
		g[NextX][NextY] = g[CurX][CurY] + 1;
		if( g[NextX][NextY]+h[NextX][NextY] <= g[CurX][CurY]+h[CurX][CurY] ){
			NextDir = i;
		}
	}
	//找出了f(n)最小的那个,赋值给当前坐标(CurX,CurY) 
	CurX += Move[NextDir].x;
	CurY += Move[NextDir].y;
	Astar_Straight(CurX,CurY);
	return ;
}	
	

int main(){
//	cout << "A* star Algorithm";
	int x,y,i,j,k;
	Initialize_Straight();				//初始化每个点的h 
	g[1][1] = 0;
	Astar_Straight(1,1);
	
	return 0;
} 
