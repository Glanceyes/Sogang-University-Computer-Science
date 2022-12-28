#include <cstdio>
#include <cstdlib>
#include "stdafx.h"
#include "WaterfallSolver.h"

int listSize = 0, lineNum, pointNum, currentPoint = 0;
Line* line = NULL;
Point* point = NULL;
Node* firstNode = NULL;
Node* currentNode = NULL;

bool initialState = false;
bool selectState = false;
bool waterfallState = false;

void F1(void){
	if(line != NULL) free(line);
	if(point != NULL) free(point);

	if(listSize > 0){
		currentNode = firstNode;

		while(currentNode != NULL){
			Node* tempNode = NULL;
			tempNode = currentNode;
			currentNode = currentNode->link;
			free(tempNode->point);
			free(tempNode);
		}

	}
	firstNode = NULL;
	line = NULL;
	point = NULL;
	listSize = 0;
	lineNum = 0;
	pointNum = 0;
	currentPoint = 0;
	initialState = false;
	selectState = false;
	waterfallState = false;
}

void F2(LPCTSTR fname){
	FILE* fp = fopen(fname, "r");
	double tempX, tempY;

	fscanf(fp, "%d\n", &lineNum);
	line = (Line*)malloc(sizeof(Line) * lineNum);
	
	for (int i = 0; i < lineNum; i++){
		fscanf(fp, "%lf %lf %lf %lf\n", &line[i].xl, &line[i].yl, &line[i].xr, &line[i].yr);
		if(line[i].xl > line[i].xr){
			tempX = line[i].xl;
			tempY = line[i].yl;
			line[i].xl = line[i].xr;
			line[i].yl = line[i].yr;
			line[i].xr = tempX;
			line[i].yr = tempY;
		}
	}

	fscanf(fp, "%d\n", &pointNum);
	point = (Point*)malloc(sizeof(Point) * pointNum);
	
	for (int i = 0; i < pointNum; i++){
		fscanf(fp, "%lf %lf\n", &point[i].x, &point[i].y);
	}
	fclose(fp);
	initialState = true;
}


void F3(void){
	Point findPoint;

	double maxInterY = 0, InterY = 0;
	int nextLine = 0, i;

	if(initialState==false) return;

	if(listSize > 0){
		currentNode = firstNode;

		while(currentNode != NULL){
			Node* tempNode = NULL;
			tempNode = currentNode;
			currentNode = currentNode->link;
			free(tempNode->point);
			free(tempNode);
		}
	}
	listSize = 0;
	firstNode = currentNode = NULL;

	findPoint.x = point[currentPoint].x;
	findPoint.y = point[currentPoint].y;

	Node* waterPath = NULL;
	firstNode = (Node*)malloc(sizeof(Node));
	firstNode->link = NULL;
	firstNode->point = (Point*)malloc(sizeof(Point));
	waterPath = firstNode;
	waterPath->point->x = findPoint.x;
	waterPath->point->y = findPoint.y;
	listSize++;

	while(1){
		maxInterY = 0;
		for(i = 0; i < lineNum; i++){
			if(!(line[i].xl <= findPoint.x && findPoint.x <= line[i].xr)) continue;
			InterY = (double)((line[i].yr - line[i].yl) / (line[i].xr - line[i].xl)) * (findPoint.x - line[i].xl) + line[i].yl;
			if(InterY >= findPoint.y) continue;
			if(maxInterY < InterY) {
				maxInterY = InterY;
				nextLine = i;
			}
		}
		if(maxInterY == 0) break;
		findPoint.y = maxInterY;
		addNode(&waterPath, &findPoint);

		if(line[nextLine].yl < line[nextLine].yr){
			findPoint.x = line[nextLine].xl;
			findPoint.y = line[nextLine].yl;
		}
		else {
			findPoint.x = line[nextLine].xr;
			findPoint.y = line[nextLine].yr;
		}
		addNode(&waterPath, &findPoint);
	}
	findPoint.y = 0;
	addNode(&waterPath, &findPoint);
}

void F4(CDC* pDC){
	CPen MyPen;
	if(initialState == false) return;

	MyPen.CreatePen(PS_SOLID, 10, RGB(0,0,255));
	pDC->SelectObject(MyPen);
	pDC->MoveTo(100, 100); pDC->LineTo(500, 100);
	pDC->MoveTo(100, 500); pDC->LineTo(500, 500);
	MyPen.DeleteObject(); 

	MyPen.CreatePen(PS_SOLID, 5, RGB(110,110,110));
	pDC->SelectObject(MyPen);
	for(int i=0;i< lineNum;i++){
		pDC->MoveTo(100 + (int)line[i].xl * 20, 500 - (int)line[i].yl * 20);
		pDC->LineTo(100 + (int)line[i].xr * 20, 500 - (int)line[i].yr * 20);
	}
	MyPen.DeleteObject(); 

	MyPen.CreatePen(PS_SOLID, 10, RGB(0,0,0));
	pDC->SelectObject(MyPen);
	for(int i=0;i < pointNum;i++){
		pDC->Ellipse(100 + (int)point[i].x * 20 - 3, 500 - (int)point[i].y * 20 - 3, 100 + (int)point[i].x * 20 + 3, 500 - (int)point[i].y * 20 + 3);
	}
}

void F5(CDC* pDC){
	if(initialState == false) return;
	if(selectState == false) return;
	CPen MyPen;
    MyPen.CreatePen(PS_SOLID, 15, RGB(255, 0, 255));
    pDC->SelectObject(MyPen);
    pDC->Ellipse(100 +(int)point[currentPoint].x * 20 - 3, 500 - (int)point[currentPoint].y * 20 - 3, 100 + (int)point[currentPoint].x *20 + 3, 500 - (int)point[currentPoint].y *20 + 3);
}

void F6(CDC* pDC){
	if(initialState == false) return;
	if(selectState == false) return;
	if(waterfallState==false) return;
	CPen MyPen;    
	Node* tempNode;
    MyPen.CreatePen(PS_SOLID, 5, RGB(0, 153, 255));
    pDC->SelectObject(MyPen);

	tempNode = firstNode;
    while(tempNode->link != NULL)
	{
		pDC->MoveTo(100 + tempNode->point->x * 20, 500 - tempNode->point->y * 20);
		pDC->LineTo(100 + tempNode->link->point->x * 20, 500 - tempNode->link->point->y * 20);
		tempNode = tempNode->link;
	}
}

void changeSelectPoint(void){
	if(initialState == false) return;
	if(selectState==true) {
		currentPoint++;
		if(currentPoint==pointNum){
			currentPoint=0;
			selectState = false;
		}
	} else {
		selectState = true;
		currentPoint = 0;
	}
}

void enableWaterFall(bool fallEnable){
	if(initialState == false) return;
	waterfallState = fallEnable;
}

void addNode(Node** waterPath, Point* findPoint){
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->link = NULL;
	newNode->point = (Point*)malloc(sizeof(Point));
	(*waterPath)->link = newNode;
	(*waterPath) = (*waterPath)->link;
	(*waterPath)->point->x = findPoint->x;
	(*waterPath)->point->y = findPoint->y;
	listSize++;
}