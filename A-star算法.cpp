#include<iostream>
#include<stdio.h>
#define MAXMATRIXSIZE 10
#define MAXSTACKSIZE  100
#define MAXDATA 999
#define QUEUESIZE 100
using namespace std;

// A*�㷨,б�߿��� 

// ����:1.ΪʲôMove������Ҫ��ֱ�ߵķ����ǰ��λ,��б�ߵķź���λ 
//		2.����õݹ�����ö���ʵ��BFS?
//		3.�ɷ���DFS����ʼ��·�� 

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
(���������㷨Ϊ��):
����ÿ���̶�������˵,g(n)�ɸ�����ȷ����,����ǰ���g(n) = ����g(n) + �Ӹ����ƶ�����ǰ���·������
(ÿ�ΰ�һ�����ӱ�Ϊ�������ӵĸ���,��ѡ���Ǹ�f(n)��С�ĸ���֮��,��Ҫ��������ܱߵĸ��ӵ�g(n),��ʽ���� ) 
(��h(n)��һ��ȷ���㷨��ȷ��������, �����������,��ÿ����Ĺ������� )
ÿ��ѡ����Χ�ĸ��ӵ�ʱ��,���ҳ�f(n)��С�ĸ���(��ΪA),Ȼ���������������A,A��g(n)���ɶ���
���A��g(n)��������ԭ����g(n)Ҫ����,��˵���ߵ�A���и��õķ���,�����һ����������A
����µ�g(n)<=ԭ��A��g(n) (һ������¶���=) ��ô��ʱѡ������A��������ȷ�� 

*/

// (1,1) --> (8,8)

struct Direct{
	int x;
	int y;
};

//Direct Move[8] = { {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0} };	// �����Ͽ�ʼ,�±�ż��б��,����ֱ�� 
//					  		 ��			   	��				��			�� 
Direct Move[8] = { {0,-1}, {1,0}, {0,1}, {-1,0}, {-1,-1}, {1,-1}, {1,1}, {-1,1} };	// i<4ֱ��,i>=4б���� 

struct QueueNode{
	int x;
	int y;
	int Distance;
};

QueueNode Queue[QUEUESIZE];
int front = -1;		//ָ�����βԪ�ص�ָ�� 
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
	front = (front+1) % QUEUESIZE;			// ���е������ʾ:����һλ�ٷ���! 
	*x = Queue[front].x;
	*y = Queue[front].y;
	*Distance = Queue[front].Distance;
	return ;
}

int isEmpty(){
	return front==rear;
}

void Initialize(){
	//��ʼ����ͼ��ÿ�����h(n),��Ϊ��ֻ���㷨�й�
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
	Mark[8][8] = 1 ;					// ���յ����� 
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
	int NextDir = 0;			//��һ���ķ���(0~3�ֱ��ĸ�����) 
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
	//�ҳ���f(n)��С���Ǹ�,��ֵ����ǰ����(CurX,CurY) 
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
	Mark[8][8] = 1 ;					// ע��Ҫ��֮ǰ���յ����� 
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
	int NextDir = 0;			//��һ���ķ���(0~3�ֱ��ĸ�����) 
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
	//�ҳ���f(n)��С���Ǹ�,��ֵ����ǰ����(CurX,CurY) 
	CurX += Move[NextDir].x;
	CurY += Move[NextDir].y;
	Astar_Straight(CurX,CurY);
	return ;
}	
	

int main(){
//	cout << "A* star Algorithm";
	int x,y,i,j,k;
	Initialize_Straight();				//��ʼ��ÿ�����h 
	g[1][1] = 0;
	Astar_Straight(1,1);
	
	return 0;
} 
