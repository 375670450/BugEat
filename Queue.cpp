#include "Queue.h"
Queue::Queue(){
    front = rear = -1;
    number = 0;
    qArray = new QueueNode[QUEUESIZE];
}

void Queue::AddQ(int x,int y,int Distance ){
//    if(front<0) front = 0;
    rear = rear+1;
	qArray[rear].x = x;
	qArray[rear].y = y;
	qArray[rear].Distance = Distance;
	number+=1;
	return ;
}
void Queue::DeleteQ(int &x,int &y,int &Distance ){
//    if(front<0){
//        cout << "Queue Empty" << endl;
//        return ;
//    }
    front = front+1;
	x = qArray[front].x;
	y = qArray[front].y;
	Distance = qArray[front].Distance;
	number-=1;
	return ;
}

void Queue::ClearQ(){
    front = rear = -1;
    number = 0;
}

int Queue::NumberofQ(){
    return number;
}

int Queue::isEmpty(){
    return front==rear;
}

Queue::~Queue(){
    number = 0;
    front = rear = -1;
    delete [] qArray;
}
