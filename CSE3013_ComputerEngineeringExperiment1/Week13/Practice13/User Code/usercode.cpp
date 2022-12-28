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

//Start of user code
typedef struct _mz{
	int chk; // �湮�ߴ��� ����
	bool right,under; // ĭ���� ������ �Ǵ� �Ʒ������� �� �� �ִ��� �̵� ������ ���� ����
} mz;
mz **DS;
char **maze;

int HEIGHT,WIDTH;
int MazeHeight, MazeWidth;
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
	
	for(int i = 0; i < HEIGHT; i++){
		for(int j = 0;j < WIDTH; j++){
			if(i % 2 == 0){
				if(maze[i][j] == '+'){
					DrawSolidBox_I((j/2)*4,(i/2)*4,(j/2)*4+1,(i/2)*4+1,1,RGB(120,0,120),RGB(120,0,120));
				}
				else if(maze[i][j] == '-'){
					DrawSolidBox_I((j/2)*4+1,(i/2)*4,(j/2)*4+4,(i/2)*4+1,1,RGB(120,0,120),RGB(120,0,120));
				}
			}
			else{
				if(maze[i][j] == '|'){
					DrawSolidBox_I((j/2)*4,(i/2)*4+1,(j/2)*4+1,(i/2)*4+4,1,RGB(120,0,120),RGB(120,0,120));
				}
			}
		}
	}

	//end of the user code
}