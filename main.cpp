#define _DEBUG
#include <graphics.h>
#include <time.h>
#include <stdlib.h>
#include "Queue.h"


/*  难点:
        1.如何随机生成一个有通路的MAZE:用递归(类似于递归走迷宫但不完全相同);
            根本思想就是使得所有的墙格周围的墙格不超过2个(这样就能使得通路一定存在且唯一?)
        2.【重点】移动DST超过两格的时候内存溢出,原因：Initialize_Straight函数中用的队列对象Q
           只在该函数中用到,因此应该作为局部变量在函数内部声明,若作为全局对象会发生内存溢出????
        3.DST改变后无法再次寻径:这种第一次运行成功第二次运行失败的函数一般都是某个变量在第二次运行前没有初始化
          还保留着第一次运行后的数据,因而运行不正常.因此要检查每个相关变量的初始化(REACH)

*/

#define HEIGHT 20
#define WIDTH  40
#define CS 15           //每个方格区域大小
#define MAXDATA 999
#define QUEUESIZE (WIDTH*HEIGHT)

/*定义要响应的按键(getch()函数返回的是虚拟键码的后两位(十六进制))*/
#define DOWN  VK_DOWN /*↓键*/
#define UP  VK_UP /*↑键*/
#define LEFT  VK_LEFT /*←键*/
#define RIGHT  VK_RIGHT /*→键*/
#define ESC  VK_ESCAPE /*退出键*/
#define SPACE VK_SPACE /*空格键*/
#define TAB VK_TAB
#define HOME VK_HOME
#define CONTROL VK_CONTROL
#define NUM1 VK_NUMPAD1
#define NUM2 VK_NUMPAD2
#define NUM3 VK_NUMPAD3


typedef struct SpaceStruct{
    int flag;
    int h;
    int g;
}Space;

Space Maze[WIDTH+1][HEIGHT+1];
int Reach = 0;
int Mark[WIDTH+1][HEIGHT+1];

int Move[4][2] = { {-1,0},{0,-1},{1,0},{0,1} };     //备选移动数组:左,上,右,下
int DstX = WIDTH-1, DstY = HEIGHT-1;
int Bug[5][2] = { {1,1},{4,1},{3,1},{2,1},{1,1}, }; //Bug的每个部位坐标信息,bug[0]是头,剩下全是身体
int BugX,BugY;
int stop = 0;
int key;
int times = 0;
Queue Steps;

void FillMazeWall();
void BuildMaze(int curX,int curY);
void Initialize_Straight();
void MoveBug(int NextX,int NextY);
void Astar_Straight(int CurX,int CurY);
//void InitBug(int nextX,int nextY);
void ShowMaze();
void ShowBug();
//void Search(){}
void ShowDst(int flag);
void MoveDst(int Dir);
void GameOver();


int main(){
    initgraph(640,480);
   	randomize();
   	FillMazeWall();
   	BuildMaze(1,1);

    ShowMaze();
    Initialize_Straight();
    Astar_Straight(Bug[0][0],Bug[0][1]);
    ShowBug();
    int ticks = 0;
    int NextX,NextY;
    int Distance = 0;
    while( !stop ){
        if( ticks>10 ){
            ShowDst(1);
        }
        if( ticks>20 ){
            ShowDst(0);
            ticks = 0;
        }
        if( Maze[DstX][DstY].flag==0 && Steps.isEmpty()==0 ){
//                getch();
                Steps.DeleteQ(NextX,NextY,Distance);
                MoveBug(NextX,NextY);
                if( Bug[0][0]==DstX && Bug[0][1]==DstY ){
                    GameOver();
                }
                Sleep(10);
            }
        while( kbhit() ){
            key = getch();
            key = key & 0xff;
            switch(key){
                case LEFT:MoveDst(0);
                        break;
                case UP:MoveDst(1);
                        break;
                case RIGHT:MoveDst(2);
                        break;
                case DOWN:MoveDst(3);
                        break;
                case ESC:stop = 1;
                        break;
            }
        }
        ticks++;
        Sleep(40);
    }

    getch();
    return 0;
}


void FillMazeWall(){
    int i,j;
    for( i=0; i<=WIDTH; i++){
        for( j=0; j<=HEIGHT; j++){
            Maze[i][j].flag = 1;               //(x,y)=1
        }
    }
}

void BuildMaze(int curX,int curY){
    //递归实现迷宫路径的随机生成
//    xyprintf(0,0,"curX = %d, curY = %d,",curX,curY);
//    getch();
    if( curX<=0 || curY<=0 || curX>=WIDTH || curY>=HEIGHT)          //越界返回     省略 DstX>=WIDTH || DstY>=HEIGHT ?
        return ;
    if( Maze[curX][curY].flag==0 )               //通路,返回
        return ;
    //不是通路,则数周边墙格数量,多于两个墙格则要打空
    int count = 0,i;
    for( i=0; i<4; i++){
        if( Maze[(curX+Move[i][0])][(curY+Move[i][1])].flag==0 )     //数的是周边的格子,所以要加上Move
            count++;
    }
    if( count>=2 )      //有两个以上的方向可以走
        return ;
    //墙格数量多于两个,随机向四周打通,【但在之前要先把当前格打通？为什么？】
    Maze[curX][curY].flag = 0;
    int flag[4] = {0};                      //标记四个方向是否移动(扩展打通)过
    int nextX,nextY;
    count = 0;
    while( count<4 ){                       //注意这里循环不用for而用count是因为要使向四个方向中的哪一个是随机的(否则每次产生的迷宫会有规律)
        i = random(4);
        if( flag[i]==1 )    //这个方向已经去过了
            continue;
        flag[i] = 1;
        count++;                        // 【无论何时都要记得改变循环变量】
        nextX = curX + Move[i][0];
        nextY = curY + Move[i][1];
        BuildMaze(nextX,nextY);
    }
    return ;
}

void ShowMaze(){
    int i,j;
    for( i=0; i<=WIDTH; i++){
        for( j=0; j<=HEIGHT; j++){
            if( Maze[i][j].flag )
                setfillcolor(WHITE);
            else
                setfillcolor(BLACK);
            bar(i*CS,j*CS,(i+1)*CS,(j+1)*CS);
        }
    }
    return ;
}

void ShowDst(int flag){
    if( flag ) setfillcolor(LIGHTBLUE);
    else if( Maze[DstX][DstY].flag ){
        setfillcolor(WHITE);
    }else{
        setfillcolor(BLACK);
    }
    bar(DstX*CS,DstY*CS,(DstX+1)*CS,(DstY+1)*CS);
    return ;
}

void MoveDst(int Dir){
    int nextX,nextY;
    nextX = DstX + Move[Dir][0];
    nextY = DstY + Move[Dir][1];
    if( nextX<=0 || nextY<=0 || nextX>=WIDTH || nextY>=HEIGHT )
        return ;
    ShowDst(0);            //Clear the old mark
    DstX = nextX;
    DstY = nextY;
    ShowDst(1);
    Reach = 0;
    if( Maze[DstX][DstY].flag==1 ) return ;
    Steps.ClearQ();
    times++;
    Initialize_Straight();
//    xyprintf(700,650,"Bug[0][0] =%d,Bug[0][1]=%d",Bug[0][0],Bug[0][1]);
    Astar_Straight(Bug[0][0],Bug[0][1]);
    return ;
}

void MoveBug(int NextX,int NextY){
    if( NextX<=0 || NextY<=0 || NextX>=WIDTH || NextY>=HEIGHT
       || Maze[NextX][NextY].flag==1 )
        return ;
    setfillcolor(BLACK);
    bar(Bug[4][0]*CS,Bug[4][1]*CS,(Bug[4][0]+1)*CS,(Bug[4][1]+1)*CS);
    Bug[0][0] = NextX;
    Bug[0][1] = NextY;
    for( int i=4; i>0; i--){
        Bug[i][0] = Bug[i-1][0];
        Bug[i][1] = Bug[i-1][1];
    }
    ShowBug();
    return ;
}


void Initialize_Straight(){
    //注意是从目标点开始init到每个点都被initialize过
	int i,j;
	Queue Q;                //队列Q只在Initialize的时候用到,因此应该作为局部变量
	for( i=0; i<=WIDTH; i++){
		for( j=0; j<=HEIGHT; j++){
			Maze[i][j].h = MAXDATA;
			Maze[i][j].g = 0;
			Mark[i][j] = Maze[i][j].flag;
		}
	}
    int CurX, CurY;
	int NextX,NextY,NextDistance;
	int Distance = 0;
    CurX = DstX;
    CurY = DstY;
	Mark[(Bug[0][0])][(Bug[0][1])] = Mark[CurX][CurY] = 1 ;					// 注意要在之前把终点和起点标记上
	Q.AddQ(CurX,CurY,Distance);
	while( !Q.isEmpty() ){
		Q.DeleteQ(CurX,CurY,Distance);
		Maze[CurX][CurY].h = Distance;
        i = 0;
//        xyprintf(0,0,"Q.front = %d,CurX = %d,CurY = %d,Distance = %d",Q.front,CurX,CurY,Distance);
//        getch();
		while( i<4 ){
			NextX = CurX + Move[i][0];
			NextY = CurY + Move[i][1];
//			xyprintf(0,20,"curX = %d,curY = %d,nextX = %d,nextY = %d,Mark[NextX][NextY] = %d",CurX,CurY,NextX,NextY,Mark[NextX][NextY]);
//            getch();
			if( !Mark[NextX][NextY] ){
//                getch();
				NextDistance = Maze[CurX][CurY].h + 1;
				Mark[NextX][NextY] = 1;
				Q.AddQ(NextX,NextY,NextDistance);
			}
			i++;
		}
	}
//	getch();
////    cleardevice();
//	for( i=0; i<=WIDTH; i++){
//		for( j=0; j<=HEIGHT; j++){
//			xyprintf(700+j*30,i*15,"%d",Maze[i][j].h);
//		}
//	}
}

void Astar_Straight(int CurX,int CurY){
    Steps.AddQ(CurX,CurY);
    if( CurX<=0 || CurY<=0 || CurX>=WIDTH || CurY >= HEIGHT){
        return ;
    }
	if( CurX==DstX && CurY==DstY ){
		Reach = 1;
		return ;
	}
//	xyprintf(0,380,"CurX=%d, CurY=%d, DstX=%d, DstY=%d",CurX,CurY,DstX,DstY);

	int NextX,NextY;
	int NextDir = 0;			//下一步的方向(0~3分别四个方向)
	NextX = CurX;
	NextY = CurY;

	for( int i=0; i<4 && !Reach; i++){
		NextX = CurX + Move[i][0];
		NextY = CurY + Move[i][1];
		Maze[NextX][NextY].g = Maze[CurX][CurY].g + 1;
		if( Maze[NextX][NextY].g + Maze[NextX][NextY].h <= Maze[CurX][CurY].g + Maze[CurX][CurY].h ){
			NextDir = i;
		}
	}
	//找出了f(n)最小的那个,赋值给当前坐标(CurX,CurY)
	CurX += Move[NextDir][0];
	CurY += Move[NextDir][1];
	Astar_Straight(CurX,CurY);
	return ;
}

void ShowBug(){
    int i,x,y;
    for( i=0; i<5; i++){
        if( i<=2 ){
            setfillcolor(RED);
        }else{
            setfillcolor(GREEN);
        }
        x = Bug[i][0];
        y = Bug[i][1];
        bar(x*CS+1,y*CS+1,(x+1)*CS-1,(y+1)*CS-1);
    }
    return ;
}

void GameOver(){
    cleardevice();
    setcolor(WHITE);
    setfont(0,0,"微软雅黑");
    xyprintf(260,220,"Game is Over");
    stop = 1;
}
