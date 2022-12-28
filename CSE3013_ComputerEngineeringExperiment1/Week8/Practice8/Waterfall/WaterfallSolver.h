#include "stdafx.h"

typedef struct{
	double xl, yl, xr, yr;
}Line;

typedef struct{
	double x, y;
}Point;

typedef struct NODE{
	Point* point;
	struct NODE* link;
}Node;

extern int listSize, lineNum, pointNum, currentPoint, selectedPoint;
extern Line* line;
extern Point* point;
extern Node* firstNode;
extern Node* currentNode;
extern bool initialState;
extern bool selectState;
extern bool waterfallState;

void F1(void);
void F2(LPCTSTR fname);
void F3(void);
void F4(CDC* pDC);
void F5(CDC* pDC);
void F6(CDC* pDC);
void changeSelectPoint(void);
void enableWaterFall(bool fallEnable);
void addNode(Node** waterPath, Point* findPoint);