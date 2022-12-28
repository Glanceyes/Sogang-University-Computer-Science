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
	int chk; // 방문했는지 저장
	bool right,under; // 칸에서 오른쪽 또는 아래쪽으로 갈 수 있는지 이동 가능한 방향 저장
} mz;

typedef struct _check {
	int x, y; // 편의상 실제 미로의 x, y 좌표와 다르게 뒤바꿈 (x: 높이 축을 기준으로 한 좌표, y: 너비 축을 기준으로 한 좌표)
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
	while(1){ // 처음 읽을 때는 미로의 너비와 높이 파악하기 위해
		if(fscanf(fp,"%c",&c) == EOF){
			if(t!='\n') // 파일은 다 읽었는데 마지막 줄에서 줄바꿈이 발생하지 않는다면 높이 증가
				HEIGHT = height + 1;
			break;
		}
		if(c == '\n'){ // 줄바꿈 문자 만나면 높이 증가
			WIDTH = width + 1;
			height++;
			width = -1;
		}
		t = c;
		width++;
	}
	fclose(fp);

	// 두 번째로 읽을 때는 미로 데이터 저장
	fp = fopen(filename, "r");
	maze = (char**)malloc(sizeof(char*) * HEIGHT);
	for(i = 0;i < HEIGHT; i++) {
		maze[i] = (char*)malloc(sizeof(char) * WIDTH);
	}

	// 실제로 미로 탐색할 때 벽에 해당하는 칸들은 필요 없으므로
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
            if(i + 2 < HEIGHT - 1 && maze[i+1][j] == ' ') // 칸의 y좌표가 미로 안에 있고 현재 위치에서 아래 있는 칸이 빈 칸이면
                DS[width][height].under = 1; // 아래로 갈 수 있음을 저장
            if(j + 2 < WIDTH - 1 && maze[i][j+1] == ' ') // 칸의 x좌표가 미로 안에 있고 현재 위치에서 오른쪽에 있는 칸이 빈 칸이면
                DS[width][height].right = 1; // 오른쪽으로 갈 수 있음을 저장
            height++;
        }
        width++;
    }
	
	stack = (check*)malloc(sizeof(check) * MazeHeight * MazeWidth + 12); // stack 메모리 할당
	queue = (check*)malloc(sizeof(check) * MazeHeight * MazeWidth + 12); // queue 메모리 할당

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

	// stack과 queue 초기화
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

	// DFS로 찾아야 하면
	if(dfs_check == 1){
		DrawSolidBox_I(1, 1, 4, 4, 1, RGB(255,255,0),RGB(255,255,0));
		DrawSolidBox_I(4 * MazeWidth -3, 4 * MazeHeight - 3, 4 * MazeWidth, 4 * MazeHeight, 1, RGB(255,0,0), RGB(255,0,0));
		// 갔던 곳 다 출력
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
		// 정답 출력
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

	// BFS로 찾아야 하면
	if(bfs_check == 1){
		DrawSolidBox_I(1,1,4,4,1, RGB(255,255,0), RGB(255,255,0));
		DrawSolidBox_I(4 * MazeWidth - 3, 4 * MazeHeight - 3, 4 * MazeWidth, 4 * MazeHeight,1, RGB(255,0,0), RGB(255,0,0));
		// 갔던 곳 다 출력
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
		// 정답 출력

		/* 
		BFS 탐색 중요도
		아래쪽 → 오른쪽 → 왼쪽 → 위쪽
		*/
		while(t.x != 0 || t.y != 0){
			temp = t;		
			DrawSolidBox_I(t.y*4+2,t.x*4+2,t.y*4+3,t.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
			if(DS[temp.x][temp.y].chk == 1){ // 아래쪽 방향으로 경로가 있으면
				temp.x--;
				DrawSolidBox_I(temp.y*4+2,temp.x*4+3,t.y*4+3,t.x*4+2,1,RGB(0,0,255),RGB(0,0,255));
			}
			else if(DS[temp.x][temp.y].chk == 2){ // 오른쪽 방향으로 경로가 있으면
				temp.y--;
				DrawSolidBox_I(temp.y*4+3,temp.x*4+2,t.y*4+2,t.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
			}
			else if(DS[temp.x][temp.y].chk == 3){ // 왼쪽 방향으로 경로가 있으면
				temp.y++;
				DrawSolidBox_I(t.y*4+3,t.x*4+2,temp.y*4+2,temp.x*4+3,1,RGB(0,0,255),RGB(0,0,255));
			}
			else if(DS[temp.x][temp.y].chk == 4){ // 위쪽 방향으로 경로가 있으면
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

	// bfs를 이미 작업한 상태에서 dfs를 실행했을 경우
	if(bfs_one == 1){
		// 미로 초기화 작업
		for(i = 0;i < MazeHeight;i++){
			for(j = 0;j < MazeWidth;j++){
				if(DS[i][j].chk != 0){
					DrawSolidBox_I(j*4+2,i*4+2,j*4+3,i*4+3,1,RGB(0, 0,0),RGB(0,0,0));
					if(i-1 >= 0 && DS[i-1][j].chk != 0 && DS[i-1][j].under == 1){ // 아래로 가는 경로가 있었으면
						DrawSolidBox_I(j*4+2,(i-1)*4+3,j*4+3,i*4+2,1,RGB(0,0,0),RGB(0,0,0));
					}
					if(j-1 >= 0 && DS[i][j-1].chk != 0 && DS[i][j-1].right == 1){ // 오른쪽으로 가는 경로가 있었으면
						DrawSolidBox_I((j-1)*4+3,i*4+2,j*4+2,i*4+3,1,RGB(0,0,0),RGB(0,0,0));
					}
				}
			}
		}
		bfs_one=0;
	}

	// 방문 초기화
	for(i = 0;i < MazeHeight;i++) {
		for(j = 0;j < MazeWidth; j++) {
			DS[i][j].chk=0;
		}
	}

	temp.x = 0;
	temp.y = 0;
	stack_cnt = 0;

	/*
	방향 탐색 순서
	아래쪽 → 오른쪽 → 왼쪽 → 위쪽
	*/

	// 처음에 stack에 시작 칸 데이터 insert
	stack[stack_cnt] = temp;
	DS[0][0].chk = 1;
	stack_cnt++;
	while(stack_cnt != 0){ // stack이 빌 때까지
		temp = stack[stack_cnt-1]; // stack에서 delete(pop)
		if(temp.x == MazeHeight - 1 && temp.y == MazeWidth - 1) { // 미로 경계까지 도달했으면 종료
			break;
		}
		if(temp.x + 1 < MazeHeight && DS[temp.x][temp.y].under == 1 && DS[temp.x+1][temp.y].chk == 0){ // 아래쪽으로 갈 수 있으면
			DS[temp.x+1][temp.y].chk=1; // 방문한 것으로 처리
			temp2.x=temp.x+1;
			temp2.y=temp.y;
			stack[stack_cnt]=temp2; // stack에 insert(push)
			stack_cnt++;
			continue;
		}
		if(temp.y+1 < MazeWidth && DS[temp.x][temp.y].right == 1 && DS[temp.x][temp.y+1].chk == 0){ // 오른쪽으로 갈 수 있으면
			DS[temp.x][temp.y+1].chk=1;  // 방문한 것으로 처리
			temp2.x = temp.x;
			temp2.y = temp.y + 1;
			stack[stack_cnt] = temp2; // stack에 insert(push)
			stack_cnt++;
			continue;
		}
		if(temp.y > 0 && DS[temp.x][temp.y-1].right == 1 && DS[temp.x][temp.y-1].chk == 0){ // 왼쪽으로 갈 수 있으면
			DS[temp.x][temp.y - 1].chk = 1; // 방문한 것으로 처리
			temp2.x = temp.x;
			temp2.y = temp.y - 1;
			stack[stack_cnt] = temp2; // stack에 insert(push)
			stack_cnt++;
			continue;
		}
		if(temp.x>0 && DS[temp.x-1][temp.y].under == 1 && DS[temp.x-1][temp.y].chk == 0){ // 위쪽으로 갈 수 있으면
			DS[temp.x - 1][temp.y].chk = 1; // 방문한 것으로 처리
			temp2.x = temp.x - 1;
			temp2.y = temp.y;
			stack[stack_cnt] = temp2; // stack에 insert(push)
			stack_cnt++;
			continue;
		}
		stack_cnt--;
	}
	dfs_one = 1;// DFS 처리했음을 알려주는 FLAG 1로
	drawBuffered();
}


/***********************************************************************
* function	: void BFS()
************************************************************************/

void BFS(){
	
	int i, j, start, finish;
	dfs_check = 0;
	bfs_check = 1;

	// dfs를 이미 작업한 상태에서 bfs를 실행했을 경우
	if(dfs_one == 1){
		// 미로 초기화 작업
		for(i = 0;i < MazeHeight; i++){
			for(j = 0;j < MazeWidth; j++){
				if(DS[i][j].chk == 1){
					DrawSolidBox_I(j * 4 + 2, i * 4 + 2, j * 4 + 3, i * 4 + 3, 1, RGB(0,0,0), RGB(0,0,0));
					if(i - 1 >= 0 && DS[i-1][j].chk == 1 && DS[i-1][j].under == 1) // 아래로 가는 경로가 있었으면
						DrawSolidBox_I(j * 4 + 2,(i - 1) * 4 + 3, j * 4 + 3, i * 4 + 2, 1, RGB(0,0,0), RGB(0,0,0));
					if(j - 1 >= 0 && DS[i][j-1].chk == 1 && DS[i][j-1].right == 1) // 오른쪽으로 가는 경로가 있었으면
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
	방향 데이터
	아래쪽: 1
	오른쪽: 2
	왼쪽: 3
	위쪽: 4
	*/

	start = 0; // queue의 끝 (queue에서 delete 가장 먼저 하는 index)
	finish = 1; // queue의 시작 (queue에서 insert 가장 먼저 하는 index)
	while(start != finish){
		temp = queue[start]; // queue에서 원소 하나 delete
		start++;
		if(temp.x == MazeHeight - 1 && temp.y == MazeWidth - 1)
			break;
		if(temp.x + 1 < MazeHeight && DS[temp.x][temp.y].under == 1 && DS[temp.x+1][temp.y].chk == 0){ // 아래쪽 방향으로 갈 수 있으면
			DS[temp.x+1][temp.y].chk = 1; // 아래쪽 방향 기록
			temp2.x = temp.x+1;
			temp2.y = temp.y;
			queue[finish] = temp2; // queue에 insert
			finish++;
		}
		if(temp.y+1 < MazeWidth && DS[temp.x][temp.y].right == 1 && DS[temp.x][temp.y+1].chk == 0){ // 오른쪽 방향으로 갈 수 있으면
			DS[temp.x][temp.y+1].chk = 2; // 오른쪽 방향 기록
			temp2.x = temp.x;
			temp2.y = temp.y+1;
			queue[finish] = temp2; // queue에 insert
			finish++;
		}
		if(temp.y > 0 && DS[temp.x][temp.y-1].right == 1 && DS[temp.x][temp.y-1].chk == 0){ // 왼쪽 방향으로 갈 수 있으면
			DS[temp.x][temp.y-1].chk = 3; // 왼쪽 방향 기록
			temp2.x = temp.x;
			temp2.y = temp.y-1;
			queue[finish] = temp2; // queue에 insert
			finish++;
		}
		if(temp.x > 0 && DS[temp.x-1][temp.y].under == 1 && DS[temp.x-1][temp.y].chk == 0){ // 위쪽 방향으로 갈 수 있으면
			DS[temp.x-1][temp.y].chk = 4; // 위쪽 방향 기록
			temp2.x = temp.x-1;
			temp2.y = temp.y;
			queue[finish] = temp2; // queue에 insert
			finish++;
		}
	}
	bfs_one = 1; // BFS 처리했음을 알려주는 FLAG 1로
	drawBuffered();
	
}