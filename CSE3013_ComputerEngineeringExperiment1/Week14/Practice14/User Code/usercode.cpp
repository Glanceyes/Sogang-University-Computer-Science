#include <stdlib.h>
#include "..\ExternDoc.h"
#include "..\UI\Zoom.h"
#include "..\UI\MsgView.h"
#include "..\Graphics\DrawFunc.h"
#include "..\Example\Example.h"

#define ERROR_NUMBER -1

//function prototype

static void drawDirect(CDC *pDC);
static void drawBuffered();

void DFS();
void BFS();

//Start of user code
typedef struct _mz{
	int chk; // �湮�ߴ��� ����
	bool right,under; // ĭ���� ������ �Ǵ� �Ʒ������� �� �� �ִ��� �̵� ������ ���� ����
} mz;

typedef struct _check {
	int x, y; // ���ǻ� ���� �̷��� x, y ��ǥ�� �ٸ��� �ڹٲ� (x: ���� ���� �������� �� ��ǥ, y: �ʺ� ���� �������� �� ��ǥ)
} check;

check *stack = NULL;
check *queue = NULL;

mz **DS = NULL;
char **maze = NULL;

int HEIGHT,WIDTH;
int MazeHeight, MazeWidth;
int stack_cnt, dfs_check = 0, bfs_check = 0, dfs_one = 0, bfs_one = 0;

#include <float.h>

/*****************************************************************
* function	: bool readFile(const char* filename)
* argument	: cons char* filename - filename to be opened
* return	: true if success, otherwise flase
* remark	: After read data file, phisycal view must be set;
*			  otherwise drawing will not be executed correctly.
*			  The window will be invalidated after readFile()
*			  returns true.
******************************************************************/

bool readFile(const char* filename){
	
	//start of the user code
	char c,t;
	int width = 0, height =0,i,j;
	FILE* fp = fopen( filename, "r");

	if(fp == NULL) 
		return false;
	while(1){ // ó�� ���� ���� �̷��� �ʺ�� ���� �ľ��ϱ� ����
		if(fscanf(fp,"%c",&c) == EOF){
			if(t!='\n') // ������ �� �о��µ� ������ �ٿ��� �ٹٲ��� �߻����� �ʴ´ٸ� ���� ����
				HEIGHT = height + 1;
			break;
		}
		if(c == '\n'){ // �ٹٲ� ���� ������ ���� ����
			WIDTH = width + 1;
			height++;
			width = -1;
		}
		t = c;
		width++;
	}
	fclose(fp);

	// �� ��°�� ���� ���� �̷� ������ ����
	fp = fopen(filename, "r");
	maze = (char**)malloc(sizeof(char*) * HEIGHT);
	for(i = 0;i < HEIGHT; i++) {
		maze[i] = (char*)malloc(sizeof(char) * WIDTH);
	}

	// ������ �̷� Ž���� �� ���� �ش��ϴ� ĭ���� �ʿ� �����Ƿ�
	MazeHeight = HEIGHT/2;
	MazeWidth = (WIDTH-1)/2;
	DS = (mz**)malloc(sizeof(mz*) * MazeHeight);
	for(i = 0;i < MazeHeight;i++) {
		DS[i] = (mz*)malloc(sizeof(mz) * MazeWidth);
	}

	for(i = 0;i < HEIGHT;i++)
		for(j=0;j<WIDTH;j++){
			fscanf(fp,"%c",&maze[i][j]);
		}
	
    width = 0;
    WIDTH--;

    for(i = 1;i < HEIGHT - 1;i += 2){
        height = 0;
        for(j = 1; j < WIDTH - 1;j += 2){
            DS[width][height].chk = DS[width][height].under = DS[width][height].right = 0;
            if(i + 2 < HEIGHT - 1 && maze[i+1][j] == ' ') // ĭ�� y��ǥ�� �̷� �ȿ� �ְ� ���� ��ġ���� �Ʒ� �ִ� ĭ�� �� ĭ�̸�
                DS[width][height].under = 1; // �Ʒ��� �� �� ������ ����
            if(j + 2 < WIDTH - 1 && maze[i][j+1] == ' ') // ĭ�� x��ǥ�� �̷� �ȿ� �ְ� ���� ��ġ���� �����ʿ� �ִ� ĭ�� �� ĭ�̸�
                DS[width][height].right = 1; // ���������� �� �� ������ ����
            height++;
        }
        width++;
    }
	
	stack = (check*)malloc(sizeof(check) * MazeHeight * MazeWidth + 12); // stack �޸� �Ҵ�
	queue = (check*)malloc(sizeof(check) * MazeHeight * MazeWidth + 12); // queue �޸� �Ҵ�

	dfs_check = 0;
	dfs_one = 0;
	bfs_check = 0;
	bfs_one = 0;

	setWindow(0,0,WIDTH * 2 + 1, HEIGHT * 2 + 1, 1);

	return true; //edit after finish this function
	//end of usercode
}

/******************************************************************
* function	: bool FreeMemory()
*
* remark	: Save user data to a file
*******************************************************************/
void freeMemory(){
	//start of the user code

	for (int i = 0; i < HEIGHT; i++){
		free(maze[i]);
	}
	free(maze);
	for (int i = 0; i < MazeHeight; i++){
		free(DS[i]);
	}
	free(DS);

	// stack�� queue �ʱ�ȭ
	free(stack);
	free(queue);

	setWindow(0, 0, WIDTH * 2 + 1, HEIGHT * 2 + 1, 1);
	//end of usercode
}

/**************************************************************
* function	: bool writeFile(const char* filename)
*
* argument	: const char* filename - filename to be written
* return	: true if success, otherwise false
* remark	: Save user data to a file
****************************************************************/
bool writeFile(const char* filename){
	//start of the user code
	bool flag;
	flag = 0;

	return flag;
	//end of usercode
}

/************************************************************************
* fucntion	: void drawMain(CDC* pDC)
*
* argument	: CDC* pDC - device context object pointer
* remark	: Main drawing function. Called by CMFC_MainView::OnDraw()
*************************************************************************/
void drawMain(CDC *pDC){
	//if direct drawing is defined
#if defined(GRAPHICS_DIRECT)
	drawDirect(pDC);
	//if buffered drawing is defined
#elif defined(GRAPHICS_BUFFERED)
	drawBuffered();
#endif
}

/************************************************************************
* function	: static void drawDirect(CDC *pDC
*
* argument	: CDC* pDC - device context object pointer
* remark	: Direct drawing routines here.
*************************************************************************/
static void drawDirect(CDC *pDC){
	//begin of user code
	//Nothing to write currently.
	//end of user code
}

/***********************************************************************
* function	: static void drawBuffered()
*
* argument	: CDC* pDC -0 device object pointer
* remark	: Buffered drawing routines here.
************************************************************************/
static void drawBuffered(){
	//start of the user code
	
	check t1, t2, t, temp;

	for(int i = 0; i < HEIGHT; i++){
		for(int j = 0;j < WIDTH; j++){
			if(i % 2 == 0){
				if(maze[i][j] == '+'){
					DrawSolidBox_I((j/2)*4,(i/2)*4,(j/2)*4+1,(i/2)*4+1,1,RGB(10,10,10),RGB(10,10,10));
				}
				else if(maze[i][j] == '-'){
					DrawSolidBox_I((j/2)*4+1,(i/2)*4,(j/2)*4+4,(i/2)*4+1,1,RGB(10,10,10),RGB(10,10,10));
				}
			}
			else{
				if(maze[i][j] == '|'){
					DrawSolidBox_I((j/2)*4,(i/2)*4+1,(j/2)*4+1,(i/2)*4+4,1,RGB(10,10,10),RGB(10,10,10));
				}
			}
		}
	}

	// DFS�� ã�ƾ� �ϸ�
	if(dfs_check == 1){
		DrawSolidBox_I(1, 1, 4, 4, 1, RGB(255,255,0),RGB(255,255,0));
		DrawSolidBox_I(4 * MazeWidth -3, 4 * MazeHeight - 3, 4 * MazeWidth, 4 * MazeHeight, 1, RGB(255,0,0), RGB(255,0,0));
		// ���� �� �� ���
		for(int i = 0;i < MazeHeight;i++){
			for(int j = 0;j < MazeWidth;j++){
				if(DS[i][j].chk==1){
					DrawSolidBox_I(j*4+2,i*4+2,j*4+3,i*4+3,1,RGB(200, 200, 200),RGB(200, 200, 200));
					if(i-1 >= 0 && DS[i-1][j].chk == 1 && DS[i-1][j].under == 1)
						DrawSolidBox_I(j*4+2,(i-1)*4+3,j*4+3,i*4+2,1,RGB(200, 200, 200),RGB(200 ,200 ,200));
					if(j-1 >= 0 && DS[i][j-1].chk == 1 && DS[i][j-1].right == 1)
						DrawSolidBox_I((j-1)*4+3,i*4+2,j*4+2,i*4+3,1,RGB(200, 200, 200),RGB(200, 200, 200));
				}
			}
		}
		// ���� ���
		for(int i = stack_cnt - 1; i >= 0; i--){
			t=stack[i];
			DrawSolidBox_I(t.y*4+2,t.x*4+2,t.y*4+3,t.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
			if(i!=0){
				if(stack[i-1].x == t.x){
					t1 = stack[i-1]; 
					t2 = t;
					if(t1.y > t2.y){ 
						t1=t; 
						t2=stack[i-1];
					}
					DrawSolidBox_I(t1.y*4+3,t1.x*4+2,t2.y*4+2,t2.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
				}
				else{
					t1 = stack[i-1]; 
					t2 = t;
					if(t1.x>t2.x){
						t1=t; 
						t2=stack[i-1];
					}
					DrawSolidBox_I(t1.y*4+2,t1.x*4+3,t2.y*4+3,t2.x*4+2,1,RGB(0,0,255),RGB(0,0,255));
				}
			}
		}
	}

	// BFS�� ã�ƾ� �ϸ�
	if(bfs_check == 1){
		DrawSolidBox_I(1,1,4,4,1, RGB(255,255,0), RGB(255,255,0));
		DrawSolidBox_I(4 * MazeWidth - 3, 4 * MazeHeight - 3, 4 * MazeWidth, 4 * MazeHeight,1, RGB(255,0,0), RGB(255,0,0));
		// ���� �� �� ���
		for(int i = 0;i < MazeHeight;i++){
			for(int j = 0;j < MazeWidth;j++){
				if(DS[i][j].chk != 0){
					DrawSolidBox_I(j*4+2,i*4+2,j*4+3,i*4+3,1,RGB(200,200,200),RGB(200,200,200));
					if(i-1>=0 && DS[i-1][j].chk != 0 && DS[i-1][j].under==1){
						DrawSolidBox_I(j*4+2,(i-1)*4+3,j*4+3,i*4+2,1,RGB(200,200,200),RGB(200,200,200));
					}
					if(j-1>=0 && DS[i][j-1].chk != 0 && DS[i][j-1].right==1){
						DrawSolidBox_I((j-1)*4+3,i*4+2,j*4+2,i*4+3,1,RGB(200,200,200),RGB(200,200,200));
					}
				}
			}
		}
		t.x = MazeHeight - 1;
		t.y = MazeWidth - 1;
		// ���� ���

		/* 
		BFS Ž�� �߿䵵
		�Ʒ��� �� ������ �� ���� �� ����
		*/
		while(t.x != 0 || t.y != 0){
			temp = t;		
			DrawSolidBox_I(t.y*4+2,t.x*4+2,t.y*4+3,t.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
			if(DS[temp.x][temp.y].chk == 1){ // �Ʒ��� �������� ��ΰ� ������
				temp.x--;
				DrawSolidBox_I(temp.y*4+2,temp.x*4+3,t.y*4+3,t.x*4+2,1,RGB(0,0,255),RGB(0,0,255));
			}
			else if(DS[temp.x][temp.y].chk == 2){ // ������ �������� ��ΰ� ������
				temp.y--;
				DrawSolidBox_I(temp.y*4+3,temp.x*4+2,t.y*4+2,t.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
			}
			else if(DS[temp.x][temp.y].chk == 3){ // ���� �������� ��ΰ� ������
				temp.y++;
				DrawSolidBox_I(t.y*4+3,t.x*4+2,temp.y*4+2,temp.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
			}
			else if(DS[temp.x][temp.y].chk == 4){ // ���� �������� ��ΰ� ������
				temp.x++;
				DrawSolidBox_I(t.y*4+2,t.x*4+3,temp.y*4+3,temp.x*4+2,1,RGB(0,0,255),RGB(0,0,255));
			}
			t=temp;
		}
		DrawSolidBox_I(2, 2, 3, 3, 1, RGB(0,0,255), RGB(0,0,255));
	}


	//end of the user code
}

/***********************************************************************
* function	: void DFS()
************************************************************************/

void DFS(){
	int i,j;
	check temp,temp2;
	dfs_check=1;
	bfs_check=0;

	// bfs�� �̹� �۾��� ���¿��� dfs�� �������� ���
	if(bfs_one == 1){
		// �̷� �ʱ�ȭ �۾�
		for(i = 0;i < MazeHeight;i++){
			for(j = 0;j < MazeWidth;j++){
				if(DS[i][j].chk != 0){
					DrawSolidBox_I(j*4+2,i*4+2,j*4+3,i*4+3,1,RGB(0, 0,0),RGB(0,0,0));
					if(i-1 >= 0 && DS[i-1][j].chk != 0 && DS[i-1][j].under == 1){ // �Ʒ��� ���� ��ΰ� �־�����
						DrawSolidBox_I(j*4+2,(i-1)*4+3,j*4+3,i*4+2,1,RGB(0,0,0),RGB(0,0,0));
					}
					if(j-1 >= 0 && DS[i][j-1].chk != 0 && DS[i][j-1].right == 1){ // ���������� ���� ��ΰ� �־�����
						DrawSolidBox_I((j-1)*4+3,i*4+2,j*4+2,i*4+3,1,RGB(0,0,0),RGB(0,0,0));
					}
				}
			}
		}
		bfs_one=0;
	}

	// �湮 �ʱ�ȭ
	for(i = 0;i < MazeHeight;i++) {
		for(j = 0;j < MazeWidth; j++) {
			DS[i][j].chk=0;
		}
	}

	temp.x = 0;
	temp.y = 0;
	stack_cnt = 0;

	/*
	���� Ž�� ����
	�Ʒ��� �� ������ �� ���� �� ����
	*/

	// ó���� stack�� ���� ĭ ������ insert
	stack[stack_cnt] = temp;
	DS[0][0].chk = 1;
	stack_cnt++;
	while(stack_cnt != 0){ // stack�� �� ������
		temp = stack[stack_cnt-1]; // stack���� delete(pop)
		if(temp.x == MazeHeight - 1 && temp.y == MazeWidth - 1) { // �̷� ������ ���������� ����
			break;
		}
		if(temp.x + 1 < MazeHeight && DS[temp.x][temp.y].under == 1 && DS[temp.x+1][temp.y].chk == 0){ // �Ʒ������� �� �� ������
			DS[temp.x+1][temp.y].chk=1; // �湮�� ������ ó��
			temp2.x=temp.x+1;
			temp2.y=temp.y;
			stack[stack_cnt]=temp2; // stack�� insert(push)
			stack_cnt++;
			continue;
		}
		if(temp.y+1 < MazeWidth && DS[temp.x][temp.y].right == 1 && DS[temp.x][temp.y+1].chk == 0){ // ���������� �� �� ������
			DS[temp.x][temp.y+1].chk=1;  // �湮�� ������ ó��
			temp2.x = temp.x;
			temp2.y = temp.y + 1;
			stack[stack_cnt] = temp2; // stack�� insert(push)
			stack_cnt++;
			continue;
		}
		if(temp.y > 0 && DS[temp.x][temp.y-1].right == 1 && DS[temp.x][temp.y-1].chk == 0){ // �������� �� �� ������
			DS[temp.x][temp.y - 1].chk = 1; // �湮�� ������ ó��
			temp2.x = temp.x;
			temp2.y = temp.y - 1;
			stack[stack_cnt] = temp2; // stack�� insert(push)
			stack_cnt++;
			continue;
		}
		if(temp.x>0 && DS[temp.x-1][temp.y].under == 1 && DS[temp.x-1][temp.y].chk == 0){ // �������� �� �� ������
			DS[temp.x - 1][temp.y].chk = 1; // �湮�� ������ ó��
			temp2.x = temp.x - 1;
			temp2.y = temp.y;
			stack[stack_cnt] = temp2; // stack�� insert(push)
			stack_cnt++;
			continue;
		}
		stack_cnt--;
	}
	dfs_one = 1;// DFS ó�������� �˷��ִ� FLAG 1��
	drawBuffered();
}


/***********************************************************************
* function	: void BFS()
************************************************************************/

void BFS(){
	
	int i, j, start, finish;
	dfs_check = 0;
	bfs_check = 1;

	// dfs�� �̹� �۾��� ���¿��� bfs�� �������� ���
	if(dfs_one == 1){
		// �̷� �ʱ�ȭ �۾�
		for(i = 0;i < MazeHeight; i++){
			for(j = 0;j < MazeWidth; j++){
				if(DS[i][j].chk == 1){
					DrawSolidBox_I(j * 4 + 2, i * 4 + 2, j * 4 + 3, i * 4 + 3, 1, RGB(0,0,0), RGB(0,0,0));
					if(i - 1 >= 0 && DS[i-1][j].chk == 1 && DS[i-1][j].under == 1) // �Ʒ��� ���� ��ΰ� �־�����
						DrawSolidBox_I(j * 4 + 2,(i - 1) * 4 + 3, j * 4 + 3, i * 4 + 2, 1, RGB(0,0,0), RGB(0,0,0));
					if(j - 1 >= 0 && DS[i][j-1].chk == 1 && DS[i][j-1].right == 1) // ���������� ���� ��ΰ� �־�����
						DrawSolidBox_I((j - 1) * 4 + 3,i * 4 + 2, j * 4 + 2, i * 4 + 3, 1, RGB(0,0,0), RGB(0,0,0));
				}
			}
		}
		dfs_one = 0;
	}
	check temp, temp2;
	for(i = 0;i < MazeHeight; i++) {
		for(j = 0;j < MazeWidth; j++) {
			DS[i][j].chk=0;
		}
	}
	temp.x = 0; temp.y = 0;

	DS[0][0].chk = 1;
	queue[0] = temp;
	
	/*
	���� ������
	�Ʒ���: 1
	������: 2
	����: 3
	����: 4
	*/

	start = 0; // queue�� �� (queue���� delete ���� ���� �ϴ� index)
	finish = 1; // queue�� ���� (queue���� insert ���� ���� �ϴ� index)
	while(start != finish){
		temp = queue[start]; // queue���� ���� �ϳ� delete
		start++;
		if(temp.x == MazeHeight - 1 && temp.y == MazeWidth - 1)
			break;
		if(temp.x + 1 < MazeHeight && DS[temp.x][temp.y].under == 1 && DS[temp.x+1][temp.y].chk == 0){ // �Ʒ��� �������� �� �� ������
			DS[temp.x+1][temp.y].chk = 1; // �Ʒ��� ���� ���
			temp2.x = temp.x+1;
			temp2.y = temp.y;
			queue[finish] = temp2; // queue�� insert
			finish++;
		}
		if(temp.y+1 < MazeWidth && DS[temp.x][temp.y].right == 1 && DS[temp.x][temp.y+1].chk == 0){ // ������ �������� �� �� ������
			DS[temp.x][temp.y+1].chk = 2; // ������ ���� ���
			temp2.x = temp.x;
			temp2.y = temp.y+1;
			queue[finish] = temp2; // queue�� insert
			finish++;
		}
		if(temp.y > 0 && DS[temp.x][temp.y-1].right == 1 && DS[temp.x][temp.y-1].chk == 0){ // ���� �������� �� �� ������
			DS[temp.x][temp.y-1].chk = 3; // ���� ���� ���
			temp2.x = temp.x;
			temp2.y = temp.y-1;
			queue[finish] = temp2; // queue�� insert
			finish++;
		}
		if(temp.x > 0 && DS[temp.x-1][temp.y].under == 1 && DS[temp.x-1][temp.y].chk == 0){ // ���� �������� �� �� ������
			DS[temp.x-1][temp.y].chk = 4; // ���� ���� ���
			temp2.x = temp.x-1;
			temp2.y = temp.y;
			queue[finish] = temp2; // queue�� insert
			finish++;
		}
	}
	bfs_one = 1; // BFS ó�������� �˷��ִ� FLAG 1��
	drawBuffered();
	
}