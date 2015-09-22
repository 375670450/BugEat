#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#define QUEUESIZE (WIDTH*HEIGHT)
#define HEIGHT 20
#define WIDTH  40


using namespace std;

struct QueueNode{
	int x;
	int y;
	int Distance;
};

class Queue{
public:
        int rear;
        int front;
        int number;
        QueueNode * qArray ;
public:
        Queue();
        void AddQ(int x,int y,int distance = 0);
        void DeleteQ(int &x,int &y,int &distance );
        void ClearQ();
        int NumberofQ();
        int isEmpty();
        ~Queue();
};
#endif // QUEUE_H
