#include "tetris.h"

static struct sigaction act, oact;

Node* newNode(char* nameData, int scoreData){
	Node* tempNode = (Node*)malloc(sizeof(Node));
	strcpy(tempNode->nodeName, nameData);
	tempNode->nodeScore = scoreData;
	tempNode->left = NULL;
	tempNode->right = NULL;
	
	return tempNode;
}


Node* sortedBST(Node* arrayData, int start, int end){
	if (start > end){
		return NULL;
	}

	int mid = (start + end)/2;
	Node* root = newNode(arrayData[mid].nodeName, arrayData[mid].nodeScore);

	root->left = sortedBST(arrayData, start, mid - 1);
	root->right = sortedBST(arrayData, mid + 1, end);

	return root;
}

Node* insertBST(Node* root, char* nameData, int scoreData){
	if (root == NULL) {
		return root = newNode(nameData, scoreData);
	}
	if (root->nodeScore <= scoreData){
		root->left = insertBST(root->left, nameData, scoreData);
	}
	if (root->nodeScore > scoreData){
		root->right = insertBST(root->right, nameData, scoreData);
	}

	return root;
}

void inOrder(Node* node, int *counter, int X, int Y){
	if(node != NULL) {
		inOrder(node->left, counter, X, Y);
		(*counter)++;
		if ((*counter) >= X && (*counter) <= Y) {
			printw("%s %d\n", node->nodeName, node->nodeScore);
		}
		inOrder(node->right, counter, X, Y);
	}
	return;
}

void inOrderWrite(FILE* fp, Node* node){
	if(node != NULL) {
		inOrderWrite(fp, node->left);
		fprintf(fp,"%s	%d\n",node->nodeName,node->nodeScore);
		inOrderWrite(fp, node->right);
	}
	return;
}

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0] = rand()%7;
	nextBlock[1] = rand()%7;
	nextBlock[2] = rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(10, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(17,WIDTH+10);
	printw("SCORE");
	DrawBox(18,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(19,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(11 + i, WIDTH + 13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){ 
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				if(tile != '.')
					attron(A_REVERSE);
				printw("%c",tile);
				if(tile != '.')
					attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate, char tile){
	if(tile == '.'){
		DrawShadow(y, x, blockID, blockRotate, 0);
		DrawBlock(y, x, blockID, blockRotate, '.');
	}
	else if(tile == ' '){
		DrawShadow(y, x, blockID, blockRotate, 1);
		DrawBlock(y, x, blockID, blockRotate, ' ');
	}
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
	writeRankFile();
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				if(f[blockY + i][blockX + j] == 1)
					return 0;
				else if(!((blockY + i >= 0) && (blockY + i < HEIGHT)))
					return 0;
				else if(!((blockX + j >= 0) && (blockX + j < WIDTH)))
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int tempX = blockX, tempY = blockY, tempRotate = blockRotate;
	switch(command){
		case KEY_UP: tempRotate = (tempRotate + 3) % 4;
			break;
		case KEY_DOWN: tempY--;
			break;
		case KEY_LEFT: tempX++;
			break;
		case KEY_RIGHT: tempX--;
			break;
	}
	DrawBlockWithFeatures(tempY, tempX, currentBlock, tempRotate, '.');
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate, ' ');
}

void BlockDown(int sig){
	// user code
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		if(blockY == -1) {
			gameOver = 1;
		}
		else {
			score += DeleteLine(field);
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
			DrawNextBlock(nextBlock);
			blockY = -1;
			blockX = (WIDTH / 2) - 2;
			blockRotate = 0;
			PrintScore(score);
		}
		DrawField();
	}
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int touched = 0;
	for(int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				if((blockY + i >= 0 && blockY + i < HEIGHT) && (blockX + j >= 0 && blockX + j < WIDTH)){
					f[blockY + i][blockX + j] = 1;
					if(blockY + i + 1 == HEIGHT || f[blockY + i + 1][blockX + j])
						touched++;
				}
			}
		}
	}
	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int counter = 0, lineNumber = 0;
	for (int i = 0; i < HEIGHT; i++){
		counter = 0;
		for (int j = 0; j < WIDTH; j++){
			if(f[i][j] == 1) counter++;
			else break;
		}
		if(counter == WIDTH){
			lineNumber++;
			for (int m = i; m > 0; m--){
				for(int n = 0; n < WIDTH; n++){
					f[m][n] = f[m - 1][n];
				}
			}
			for (int k = 0; k <WIDTH; k++){
				f[0][k] = 0;
			}
		}
	}
	return lineNumber * lineNumber * 100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate, int blockFlag){
	// user code
	int i = 0;
	while(CheckToMove(field, blockID, blockRotate, y + i, x) == 1){
		i++;
	}
	if(blockFlag == 1){
		DrawBlock(y + i - 1, x, blockID, blockRotate, '/');
	}
	else if(blockFlag == 0){
		DrawBlock(y + i - 1, x, blockID, blockRotate, '.');
	}
}

void createRankList(){
	// user code
	FILE* fp = fopen("rank.txt", "r");
	if(fp == NULL) return;
	int rankNum, rankScore;
	char rankName[NAMELEN];
	Node* nodeArray = NULL;

	fp = fopen("rank.txt", "r");
	fscanf(fp, "%d\n", &rankNum);

	dataNum = rankNum;
	nodeArray = (Node*)malloc(sizeof(Node) * rankNum);

	for(int i = 0; i < rankNum; i++){
		fscanf(fp, "%s	%d\n", rankName, &rankScore);
		strcpy(nodeArray[i].nodeName, rankName);
		nodeArray[i].nodeScore = rankScore;
	}

	BSTroot = sortedBST(nodeArray, 0, rankNum - 1);
	free(nodeArray);
	fclose(fp);
}

void rank(){
	// user code
	int command,X=0,Y=0, counter = 0;
	clear();
	move(0,0);
	noecho();
	printw("1. list ranks form X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	command=wgetch(stdscr);
	switch(command){
		case '1':{
				echo();
				printw("X: ");
				scanw("%d",&X);
				printw("Y: ");
				scanw("%d",&Y);
				printw("      name      |    score    \n");
				printw("------------------------------\n");
				if(X == 0 && Y == 0) {
					X = 1; 
					Y = 5;
				}
				else if(X == 0) { 
					X = 1; 
					Y = Y;
				}
				else if(Y == 0) {
					X = X; 
					Y = X + 4;
				}
				else if(X < 0 || Y < 0)
				{
					printw("search failure\n");
					return;
				}
				
				if(X > Y){
					printw("search failure: no rank in the list\n");
				}
				else{
					inOrder(BSTroot, &counter, X, Y);
				}
				break;
				}
		case '2':
		case '3':
		default: break;
	}
	getch();
}

void writeRankFile(){
	// user code
	FILE* fp;
	fp=fopen("rank.txt","w");
	fprintf(fp,"%d\n",dataNum);
	inOrderWrite(fp, BSTroot);
	fclose(fp);
}

void newRank(int score){
	// user code
	char nameString[NAMELEN];
	clear();
	move(0,0);
	echo();
	printw("your name: ");
	scanw("%s", nameString);
	if (BSTroot != NULL){
		insertBST(BSTroot, nameString, score);
	}
	else {
		BSTroot = insertBST(BSTroot, nameString, score);
	}
	dataNum++;
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
