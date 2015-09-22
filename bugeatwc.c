/****************************************************/
/* BugEat in Watcom C                               */
/* copyright (C) Black White.  Mar 1, 2014          */
/* email: iceman@zju.edu.cn                         */
/* web: http://blackwhite.8866.org/bhh              */
/* This program was originally designed for running */
/* in Turbo C, and now it is adapted to Watcom C.   */
/****************************************************/
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#define HEAD 0xEE
#define NECK 0xF7
#define BODY 0x0F
#define WALL 0xB2
#define RED 0x0C
#define YELLOW 0x0E
#define GREEN 0x0A
#define WHITE 0x07
#define UP 0x48
#define DOWN 0x50
#define LEFT 0x4B
#define RIGHT 0x4D
#define ESC 0x01
#define RIGHT_BOUND 40
#define BOTTOM_BOUND 12
#define DELAY_TICKS 2

typedef struct
{
   unsigned char shape, color;
   int fx, fy;
   unsigned int g, h, f;
} TILE;  /* a node for maze */

typedef struct
{
   unsigned char shape, color;
   int x, y;
   unsigned char old_shape, old_color;
} NODE;  /* a node for bug */

typedef struct
{
   int x, y;
} POINT; /* a node for open list */

typedef void interrupt (*InterruptFunctionPointer)(void);
InterruptFunctionPointer old_8h, old_9h;
int ticks = 0, stop = 0;
int tx=RIGHT_BOUND, ty=BOTTOM_BOUND;
NODE old_target;

TILE  maze[80][25];
POINT open_list[80*25], route[80*25];
int   open_count = 0, route_count = 0;
char  mark[80][25];
NODE  bug[]={{HEAD, RED}, {NECK, YELLOW},
   {BODY,GREEN}, {BODY,GREEN}, {BODY,GREEN}};

#define N (sizeof(bug)/sizeof(bug[0]))

void draw_char(int x, int y, unsigned char shape, unsigned char color);
void fill_maze_with_wall(void);
void build_maze(int x0, int y0, int x1, int y1);
int  search_target_xy(int *px, int *py);
void show_maze(int x0, int y0, int x1, int y1);
int  init_bug(int x, int y, int i);
void draw_bug(int x, int y);
int  move_bug(int x0, int y0, int x1, int y1);
int  in_open_list(int x, int y);
int  search_best_in_open_list(int *px, int *py);
void clear_bug(void);
void show_route(void);
void my_delay(int t);
void interrupt int_9h(void);
void interrupt int_8h(void);
void recover_sth_under_old_target(void);
void save_sth_under_target(void);
void draw_target(void);
void bug_cry(void);

main()
{
   randomize();
   fill_maze_with_wall();
   build_maze(1, 1, RIGHT_BOUND, BOTTOM_BOUND);
   search_target_xy(&tx, &ty);
   memset(mark, 0, sizeof(mark));
   show_maze(0, 0, RIGHT_BOUND+1, BOTTOM_BOUND+1);
   save_sth_under_target();
   draw_target();
   bioskey(0);
   init_bug(1, 1, 0);
   old_8h = getvect(8);
   old_9h = getvect(9);
   setvect(8, int_8h);
   setvect(9, int_9h);
   while(!stop)
   {
      if(move_bug(bug[0].x, bug[0].y, tx, ty) == 1) /* target has been reached */
      {
         bug_cry();
         break;
      }
   }
   setvect(8, old_8h);
   setvect(9, old_9h);
   bioskey(0);
}

void fill_maze_with_wall(void)
{
   int x, y;
   for(y=0; y<25; y++)
   {
      for(x=0; x<80; x++)
      {
         maze[x][y].shape = WALL;
         maze[x][y].color = WHITE;
      }
   }
}

void build_maze(int x0, int y0, int x1, int y1)         //关键函数:随机生成一定含通路的地图
{
   int d[4][2] = {{-1,0},{0,1},{1,0},{0,-1}}, i, count;
   int flag[4] = {0,0,0,0};
   if(x0<=0 || y0<=0 || x0>x1 || y0>y1)
      return;
   if(maze[x0][y0].shape == ' ')                        //如果是本格是通的直接返回
      return;
   count = 0;
   //如果本格不是通的
   for(i=0; i<4; i++)
   {
      if(maze[x0+d[i][0]][y0+d[i][1]].shape == ' ')
         count++;
   }
   if(count >= 2)   //一个墙格周围有两个以上方向可走,则返回
      return;
   maze[x0][y0].shape = ' ';                            //如果本墙格没有超过2个方向是通的,继续往下执行
   maze[x0][y0].color = WHITE;
   count = 0;
   while(count < 4)                                     //向四个方向随机扩展
   {
      i = random(4);
      if(flag[i] == 1)
         continue;
      flag[i] = 1;
      count++;
      build_maze(x0+d[i][0], y0+d[i][1], x1, y1);
   }
}

int search_target_xy(int *px, int *py)
{
   int k, d[8][2]={{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1}};
   int oldx, oldy;
   if(*px < 0 || *py < 0 || *px > RIGHT_BOUND || *py > BOTTOM_BOUND)
      return 0;
   if(mark[*px][*py] == 1)
      return 0;
   mark[*px][*py] = 1;
   if(maze[*px][*py].shape == ' ')
      return 1;
   oldx = *px;
   oldy = *py;
   for(k=0; k<8; k++)
   {
      *px = oldx + d[k][0];
      *py = oldy + d[k][1];
      if(search_target_xy(px, py) == 1)
         return 1;
   }
   return 0;
}

void show_maze(int x0, int y0, int x1, int y1)
{
   int x, y;
   for(y=y0; y<=y1; y++)
   {
      for(x=x0; x<=x1; x++)
      {
         draw_char(x, y, maze[x][y].shape, maze[x][y].color);
      }
   }
}

void draw_char(int x, int y, unsigned char shape, unsigned char color)
{
   putpixel(x, y, color<<8|shape);
}

int init_bug(int x, int y, int i)
{
   int k, nx, ny;
   int d[4][2]={{-1,0},{0,1},{1,0},{0,-1}};
   char *p = _vp, *pnode;
   if(x<0 || y<0 || x>RIGHT_BOUND || y>BOTTOM_BOUND)
      return 0;
   if(maze[x][y].shape == WALL)
      return 0;
   for(k=0; k<i; k++)
   {
      if(bug[k].x == x && bug[k].y == y)
         return 0;
   }
   bug[i].x = x;
   bug[i].y = y;
   pnode = p + (bug[i].y*80+bug[i].x)*2;
   bug[i].old_shape = *pnode;
   bug[i].old_color = *(pnode+1);
   if(i == N)
      return 1;
   i++;
   for(k=0; k<4; k++)
   {
      nx = x+d[k][0];
      ny = y+d[k][1];
      if(init_bug(nx, ny, i) == 1)
         return 1;
   }
   return 0;
}

int move_bug(int x0, int y0, int x1, int y1)
{
   int k, nx, ny, sx, sy;
   int d[4][2]={{-1,0},{0,1},{1,0},{0,-1}};
   if(x0<0 || y0<0 || x0>RIGHT_BOUND || y0>BOTTOM_BOUND)
      return 0;
   if(maze[x0][y0].shape == WALL)
      return 0;
   memset(mark, 0, sizeof(mark));
   maze[x0][y0].fx = -1;
   maze[x0][y0].fy = -1; /* This is the root node, it has no father */
   maze[x0][y0].g = 0;
   maze[x0][y0].h = abs(x1-x0)+abs(y1-y0);
   maze[x0][y0].f = maze[x0][y0].g + maze[x0][y0].h;
   sx = x0;
   sy = y0;
   open_count = 0;
   while(!mark[x1][y1])
   {
      mark[sx][sy] = 1;
      for(k=0; k<4; k++)
      {
         nx = sx+d[k][0];
         ny = sy+d[k][1];
         if(nx <0 || ny < 0 || nx > RIGHT_BOUND || ny > BOTTOM_BOUND)
            continue;
         if(maze[nx][ny].shape == WALL || mark[nx][ny] == 1)
            continue;
         if(in_open_list(nx, ny))
         {
            if(maze[sx][sy].g + 1 < maze[nx][ny].g)
            {
               maze[nx][ny].fx = sx;
               maze[nx][ny].fy = sy;
               maze[nx][ny].g = maze[sx][sy].g + 1;
               maze[nx][ny].f = maze[nx][ny].g + maze[nx][ny].h;
            }
            continue;
         }
         maze[nx][ny].fx = sx;
         maze[nx][ny].fy = sy;
         maze[nx][ny].g = maze[sx][sy].g + 1;
         maze[nx][ny].h = abs(x1 - nx) + abs(y1 - nx);
         maze[nx][ny].f = maze[nx][ny].g + maze[nx][ny].h;
         open_list[open_count].x = nx;
         open_list[open_count].y = ny;
         open_count++;
      }
      if(search_best_in_open_list(&sx, &sy) == 0)
         break;
   }
   if(!mark[x1][y1])
      return 0;
   route_count = 0;
   nx = x1;
   ny = y1;
   do
   {
      int oldx, oldy;
      route[route_count].x = nx;
      route[route_count].y = ny;
      oldx = nx;
      oldy = ny;
      nx = maze[oldx][oldy].fx;
      ny = maze[oldx][oldy].fy;
      route_count++;
   } while(nx != -1 || ny != -1);
   k = route_count - 1;
   do
   {
      if(stop)                 /* If Esc is pressed, */
         return 0;             /* then mission is aborted. */
      if(x1 != tx || y1 != ty) /* If target has been changed, */
         return 0;             /* then mission should be restarted. */
      draw_bug(route[k].x, route[k].y);
      my_delay(DELAY_TICKS);
      k--;
   } while(k >= 0);
   return 1;
}

int in_open_list(int x, int y)
{
   int i;
   for(i=0; i<open_count; i++)
   {
      if(mark[open_list[i].x][open_list[i].y]) /* This node has been in close list */
         continue;
      if(open_list[i].x == x && open_list[i].y == y)
         return 1;
   }
   return 0;
}

int search_best_in_open_list(int *px, int *py)
{
   int i, n;
   unsigned int f;
   n = open_count - 1;
   if(mark[open_list[n].x][open_list[n].y])
      n = -1; /* 1st node does not exist */
   f = -1;    /* set an initial maximum value */
   for(i=open_count - 1; i>=0; i--)
   {  /* Search backward, rightmost node with same value has the priority */
      if(mark[open_list[i].x][open_list[i].y])
         continue; /* Because this node is in close list, it's out of search range */
      if(maze[open_list[i].x][open_list[i].y].f < f)
      {
         f = maze[open_list[i].x][open_list[i].y].f;
         n = i;
      }
   }
   if(n == -1)
      return 0;  /* If there is no node left, then the target is unreachable. */
   *px = open_list[n].x;
   *py = open_list[n].y;
   return 1;
}

void draw_bug(int x, int y)
{
   int i;
   char *p = _vp, *pnode;
   for(i=0; i<N; i++) /* clear bug at old location */
   {
      pnode = p + (bug[i].y*80+bug[i].x)*2;
      *pnode = bug[i].old_shape;
      *(pnode+1) = bug[i].old_color;
   }
   if(bug[0].x != x || bug[0].y != y)
   {
      for(i=N-1; i>=1; i--) /* update bug's location */
      {
         bug[i].x = bug[i-1].x;
         bug[i].y = bug[i-1].y;
      }
      bug[0].x = x;
      bug[0].y = y;
   }
   for(i=0; i<N; i++) /* save shape & color overridden by bug */
   {
      pnode = p + (bug[i].y*80+bug[i].x)*2;
      bug[i].old_shape = *pnode;
      bug[i].old_color = *(pnode+1);
   }
   for(i=N-1; i>=0; i--) /* draw new bug */
   {
      pnode = p + (bug[i].y*80+bug[i].x)*2;
      *pnode = bug[i].shape;
      *(pnode+1) = bug[i].color;
   }
}

void clear_bug(void)
{
   int i;
   char *p = _vp, *pnode;
   for(i=0; i<N; i++) /* clear bug at old location */
   {
      pnode = p + (bug[i].y*80+bug[i].x)*2;
      *pnode = bug[i].old_shape;
      *(pnode+1) = bug[i].old_color;
   }
}

void show_route(void)
{
   int k;
   for(k=route_count-1; k>=0; k--)
   {
      draw_char(route[k].x, route[k].y, ' ', 0x47);
   }
}

void interrupt int_8h(void)
{
   if(ticks != 0)
      --ticks;
   (*old_8h)();
}

void interrupt int_9h(void)
{
    unsigned char key;
    key = inportb(0x60);     /* read key code */
    if( key == 0xE0 || key == 0xE1) /* KeyExtend */
       ;
    else if( (key & 0x80) == 0x80 ) /* KeyUP */
    {
       if(key == (ESC|0x80)) /* Esc key is released */
          stop = 1;          /* Set the stop flag */
    }
    else                     /* KeyDown */
    {
       switch(key)
       {
       case UP:
          if(ty-1 <= 0)
             break;
          --ty;
          draw_target();
          break;
       case DOWN:
          if(ty+1 > BOTTOM_BOUND)
             break;
          ++ty;
          draw_target();
          break;
       case LEFT:
          if(tx-1 <= 0)
             break;
          --tx;
          draw_target();
          break;
       case RIGHT:
          if(tx+1 > RIGHT_BOUND)
             break;
          ++tx;
          draw_target();
          break;
       }
    }
    outportb(0x61, inportb(0x61) | 0x80);
    outportb(0x61, inportb(0x61) & 0x7F); /* respond to keyboard */
    outportb(0x20, 0x20); /* End Of Interrupt */
}

void my_delay(int t)
{
   ticks = t;
   while(ticks != 0)
      ;
}

void save_sth_under_target(void)
{
   unsigned char *p = _vp + (ty*80+tx)*2;
   old_target.old_shape = p[0];
   old_target.old_color = p[1];
   old_target.x = tx;
   old_target.y = ty;
}

void recover_sth_under_old_target(void)
{
   unsigned char *p = _vp +
      (old_target.y*80+old_target.x)*2;
   p[0] = old_target.old_shape;
   p[1] = old_target.old_color;
}

void draw_target(void)
{
   unsigned char *p = _vp + (ty*80+tx)*2;
   recover_sth_under_old_target();
   save_sth_under_target();
   p[0] = ' ';  /* draw new target */
   p[1] = 0xB7; /* cyan background, white foreground */
}

void bug_cry(void)
{
   unsigned int i;
   /* This sound sample is obtained from the game PC-MAM. */
   static unsigned int f[]=
   {
      0x0002, 0x0002, 0x08E3, 0x0A28, 0x0BD9, 0x0D55, 0x11C6, 0x0BD9,
      0x0D55, 0x0F3C, 0x11C6, 0x13FF, 0x1AA9, 0x11C6, 0x13FF, 0x16DA,
      0x1AA9, 0x1DFE, 0x27FE, 0x1AA9, 0x1DFE, 0x2247, 0x27FE, 0x2CFD,
      0x3BFC, 0x27FE, 0x0002, 0x05F0, 0x03F6, 0x0002, 0x05F0, 0x03F6,
      0x0002, 0x0002
   };
   outportb(0x61, inportb(0x61) | 3); /* turn on the speaker */
   for(i=0; i<sizeof(f)/sizeof(f[0]); i++)
   {
      outportb(0x43, 0xB6);
      outportb(0x42, f[i] &0xFF);     /* set frequency */
      outportb(0x42, f[i] >> 8);
      my_delay(1);
   }
   outportb(0x61,inportb(0x61)&~3);   /* turn off the speaker */
}
