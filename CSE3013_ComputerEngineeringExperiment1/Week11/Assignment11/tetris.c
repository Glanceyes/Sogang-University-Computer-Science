#include "tetris.h"

static struct sigaction act, oact;

Node* newNode(char* nameData, int scoreData, int nodeRank){
	Node* tempNode = (Node*)malloc(sizeof(Node));
	strcpy(tempNode->nodeName, nameData);
	tempNode->nodeScore = scoreData;
	tempNode->left = NULL;
	tempNode->right = NULL;
	tempNode->nodeRank = 0;
	
	return tempNode;
}

Node* sortedBST(Node* arrayData, int start, int end){
	if (start > end){
		return NULL;
	}

	int mid = (start + end)/2;
	Node* root = newNode(arrayData[mid].nodeName, arrayData[mid].nodeScore, mid + 1);

	root->left = sortedBST(arrayData, start, mid - 1);
	root->right = sortedBST(arrayData, mid + 1, end);

	return root;
}

Node* insertBST(Node* root, char* nameData, int scoreData){
	if (root == NULL) {
		return root = newNode(nameData, scoreData, 0);
	}
	if (root->nodeScore <= scoreData){
		root->left = insertBST(root->left, nameData, scoreData);
	}
	if (root->nodeScore > scoreData){
		root->right = insertBST(root->right, nameData, scoreData);
	}

	return root;
}

Node* minValueInSubBST(Node* node){
	Node* current = node;
	while (current->left != NULL) {
		current = current->left;
	}
	return current;
}

void inOrderUpdateRank(Node* node, int *updateRank){
	if(node != NULL) {
		inOrderUpdateRank(node->left, updateRank);
		(*updateRank)++;
		node->nodeRank = *updateRank;
		inOrderUpdateRank(node->right, updateRank);
	}
	return;
}

Node* deleteBST(Node* root, int rankData, int* deleteFlag){
	if (root == NULL) return root;

	if (rankData < root->nodeRank) {
		root->left = deleteBST(root->left, rankData, deleteFlag);
	}
	else if (rankData > root->nodeRank) {
		root->right = deleteBST(root->right, rankData, deleteFlag);
	}
	else {
		if (root->left == NULL) {
			Node* temp = root->right;
			free(root);
			*deleteFlag = 1;
			return temp;
		}
		else if(root->right == NULL){
			Node* temp = root->left;
			free(root);
			*deleteFlag = 1;
			return temp;
		}

		Node* temp = minValueInSubBST(root->right);
		strcpy(root->nodeName, temp->nodeName);
		root->nodeScore = temp->nodeScore;
		root->right = deleteBST(root->right, temp->nodeRank, deleteFlag);
		*deleteFlag = 1;
	}
	return root;
}

void inOrder(Node* node, int *counter, int X, int Y){
	if(node != NULL) {
		inOrder(node->left, counter, X, Y);
		(*counter)++;
		if ((*counter) >= X && (*counter) <= Y) {
			printw("%-16s|  %d\n", node->nodeName, node->nodeScore);
		}
		inOrder(node->right, counter, X, Y);
	}
	return;
}

void inOrderSearchName(Node* node, char* findName, int* findFlag){
	if(node != NULL) {
		inOrderSearchName(node->left, findName, findFlag);
		if (strcmp(node->nodeName, findName) == 0) {
			printw("%-16s|  %d\n", node->nodeName, node->nodeScore);
			*findFlag = 1;
		}
		inOrderSearchName(node->right, findName, findFlag);
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

void FreeTree(Node* node){
    if (node != NULL) {
		FreeTree(node->right);
		FreeTree(node->left);
		free(node);
	}
}

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	start_color();
	for(int i = 0; i < 7; i++) {
		init_pair(blockColor[i], blockColor[i], COLOR_BLACK);
	}

	createRankList();
	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RCMD: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	FreeTree(BSTroot);
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for (j = 0 ; j < HEIGHT; j++) {
		for(i = 0; i < WIDTH; i++) {
			field[j][i] = 0;
		}
	}

	for (i = 0; i < BLOCK_NUM; i++) {
		nextBlock[i] = rand() % 7;
	}

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	makeRecommendTree();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
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
	if (recommendPlaying == 0){
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
	}
	else if (recommendPlaying == 1) {
		switch(command){
		case 'q':
		case 'Q':
			command = QUIT;
			break;
		default:
			command = KEY_DOWN;
			break;
		}

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
			if(recommendPlaying == 0) {
				blockY++;
			}
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
	if(drawFlag) DrawChange(field ,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i, j;
	for(j = 0;j < HEIGHT; j++){
		move(j + 1, 1);
		for(i = 0; i < WIDTH; i++){
			if(field[j][i]){
				attron(A_REVERSE|COLOR_PAIR(field[j][i]));
				printw(" ");
				attroff(A_REVERSE|COLOR_PAIR(field[j][i]));
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(19, WIDTH + 11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4 + i, WIDTH + 13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE|COLOR_PAIR(blockColor[nextBlock[1]]));
				printw(" ");
				attroff(A_REVERSE|COLOR_PAIR(blockColor[nextBlock[1]]));
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(11 + i, WIDTH + 13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){ 
				attron(A_REVERSE|COLOR_PAIR(blockColor[nextBlock[2]]));
				printw(" ");
				attroff(A_REVERSE|COLOR_PAIR(blockColor[nextBlock[2]]));
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1, j+x+1);
				attron(A_REVERSE|COLOR_PAIR(blockColor[blockID]));
				printw("%c",tile);
				attroff(A_REVERSE|COLOR_PAIR(blockColor[blockID]));
			}
		}
	}

	move(HEIGHT,WIDTH+10);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawRecommend(recommendY, recommendX, blockID, recommendR);
	if (recommendPlaying != 1) {
		DrawShadow(blockY, blockX, blockID, blockRotate);
	}
	DrawBlock(blockY, blockX, blockID, blockRotate, ' ');
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

	if(recommendPlaying == 1){
		start=time(NULL);
	}

	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			if(recommendPlaying == 1){
				stop = time(NULL);
				duration = (double)difftime(stop,start);
			}
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!\n");
			if(recommendPlaying == 1){
				move(25, 0);
				printw("score(t): %d  |  time(t): %d sec\n", score, (int)duration);
				move(26, 0);
				printw("score(t)/time(t) = %.3lf", (double)score /duration);
			}
			refresh();
			getch();

			return;
		}
	}while(!gameOver);
	if(recommendPlaying == 1){
		stop = time(NULL);
		duration = (double)difftime(stop,start);
	}
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	if(recommendPlaying == 1){
		move(25, 0);
		printw("score(t): %d time(t): %.3lf\n", score, duration);
		move(26, 0);
		printw("score(t)/time(t) = %.3lf", (double)score /duration);
	}
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

void DrawFallingBlock() {
	int i, j;
	if(fallingBlock < 0) {
		return;
	}
	for(i = 0; i < 4; i++) {
		move(5 + i + 6, WIDTH + 13);
		for(j = 0; j < 4; j++) {
			if(block[fallingBlock][0][i][j]) {
				attron(A_REVERSE|COLOR_PAIR(blockColor[fallingBlock]));
				printw(" ");
				attroff(A_REVERSE|COLOR_PAIR(blockColor[fallingBlock]));
			}
			else {
				printw(" ");
			}
		}
	}
}

void FallingBlock(void) {
	int temp, i;
	if(fallingBlock < 0) {
		fallingBlock = nextBlock[0];
		for (i = 0; i < BLOCK_NUM - 1; i++) {
			nextBlock[i] = nextBlock[i + 1];
		}
		nextBlock[BLOCK_NUM - 1] = rand() % 7;
	}
	else {
		temp = fallingBlock;
		fallingBlock = nextBlock[0];
		nextBlock[0] = temp;
	}

	DrawFallingBlock();
	DrawNextBlock(nextBlock);
	blockY = -1;
	blockX = WIDTH/2 - 2;
	blockRotate = 0;
	makeRecommendTree();
	DrawField();
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int cblockY, int cblockX){
	// user code
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				if(!((cblockY + i >= 0) && (cblockY + i < HEIGHT)))
					return 0;
				if(!((cblockX + j >= 0) && (cblockX + j < WIDTH)))
					return 0;
				if(f[cblockY + i][cblockX + j])
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	DrawField();
	DrawFallingBlock();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

void BlockDown(int sig){
	// user code
	if(recommendPlaying == 1) {
		blockY = recommendY;
		blockX = recommendX;
		blockRotate = recommendR;
	}

	if(CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1) {
		blockY++;
		DrawChange(field, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		if(!CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX)) {
			gameOver = 1;
			timed_out = 0;
			return;
		}
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++)
                if (block[nextBlock[0]][blockRotate][i][j] && blockY + i < 0) {
                    gameOver = 1;
					timed_out = 0;
					return;
				}
		}
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		for (int i = 0; i < BLOCK_NUM - 1; i++) {
			nextBlock[i] = nextBlock[i + 1];
		}
		nextBlock[BLOCK_NUM - 1] = rand() % 7;
		DrawNextBlock(nextBlock);
		blockY = -1;
		blockX = (WIDTH / 2) - 2;
		blockRotate = 0;
		makeRecommendTree();
		PrintScore(score);
		DrawField();
	}
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int touched = 0, recommendTouched = 0, bx[] = {-1, 0, 1}, by[] = {0, 1, 0};

	for(int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				if((blockY + i >= 0 && blockY + i < HEIGHT) && (blockX + j >= 0 && blockX + j < WIDTH)){
					f[blockY + i][blockX + j] = blockColor[currentBlock];
					if(recommendPlaying == 1 && recommendTreeSearchFlag == 1){
						for(int k = 0; k < 3; k++) {
							if(blockY + i + by[k] >= HEIGHT) {
								recommendTouched += BOTTOM_WEIGHT;
							}
							else if(blockX + j + bx[k] < 0 || blockX + j + bx[k] >= WIDTH) {
								recommendTouched += SIDE_WEIGHT;
							}
							else if(blockY + i + by[k] >= 0 && field[blockY + i + by[k]][blockX + j + bx[k]]) {
								recommendTouched += TOUCHED_WEIGHT;
							}
						}
					
					}
					else{
						if(blockY + i + 1 >= HEIGHT || f[blockY + i + 1][blockX + j])
							touched++;
					}
				}
			}
		}
	}
	if(recommendPlaying == 1  && recommendTreeSearchFlag == 1){
		return recommendTouched;
	}
	else {
		return touched * 10;
	}
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int counter = 0, lineNumber = 0, i, j;
	for (i = 0; i < HEIGHT; i++){
		counter = 0;
		for (j = 0; j < WIDTH; j++){
			if(f[i][j] == 0) break;
		}
		if(j == WIDTH){
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

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int i = 0;
	while(CheckToMove(field, blockID, blockRotate, y + i, x) == 1){
		i++;
	}
	DrawBlock(y + i - 1, x, blockID, blockRotate, '/');
}

void createRankList(){
	// user code
	FILE* fp = fopen("rank.txt", "r");
	if(fp == NULL) return;
	int rankNum, rankScore, updateRank = 0;
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
	inOrderUpdateRank(BSTroot, &updateRank);
	free(nodeArray);
	fclose(fp);
}

void rank(){
	// user code
	int command,X=0,Y=0, counter = 0, findFlag = 0, deleteFlag = 0, updateRank = 0, findRank;
	char findName[NAMELEN + 1];
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
		case '2':{
			echo();
			printw("Input the name: ");
			scanw("%s", findName);
			printw("      name      |    score    \n");
			printw("------------------------------\n");
			inOrderSearchName(BSTroot, findName, &findFlag);
			if(findFlag == 0){
				printw("search failure: no name in the list\n");
			}
			break;
		}
		case '3':{
			echo();
			printw("Input the rank: ");
			scanw("%d",&findRank);
			deleteBST(BSTroot, findRank, &deleteFlag);
			if (deleteFlag == 0) {
				printw("search failure: the rank not in the list.\n");
			}
			else {
				printw("result: the rank deleted.\n");
				inOrderUpdateRank(BSTroot, &updateRank);
				dataNum--;
				writeRankFile();
			}
			break;
		}
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
	int updateRank = 0;
	clear();
	move(0,0);
	echo();
	printw("your name: ");
	scanw("%s", nameString);
	if (BSTroot != NULL){
		insertBST(BSTroot, nameString, score);
		inOrderUpdateRank(BSTroot, &updateRank);
	}
	else {
		BSTroot = insertBST(BSTroot, nameString, score);
		inOrderUpdateRank(BSTroot, &updateRank);
	}
	dataNum++;
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

int makeRecommendTree(){
	RecNode node;
	RecNode* root = &node;
	memcpy(root->rfield, field, sizeof(field));
	root->lv = 0;
	for (int i = 0; i < MAX_BLOCKS; i++) {
		maxValue[i] = -1;
		maxPath[i][0] = -1;
		maxPath[i][1] = -1;
		maxPath[i][2] = -1;
	}
	recommendX = recommendY = recommendR = 0;
	recommendTreeSearchFlag = 1;
	if(VISIBLE_BLOCKS >= 3){
		modified_recommend(root);
	}
	else {
		recommend(root);
	}
	recommendTreeSearchFlag = 0;
	return 0;
}

int recommend(RecNode* root){
	RecNode parentNode;
	RecNode* childNode = &parentNode;
	int rBlockX, rBlockY, rBlockR, rScore, result = -1;

	for(rBlockR = 0; rBlockR < 4; rBlockR++) {
		for(rBlockX = -2; rBlockX < WIDTH + 3; rBlockX++) {
			if(!CheckToMove(root->rfield, nextBlock[root->lv], rBlockR, 0, rBlockX)) {
				continue;
			}

			memcpy(childNode->rfield, root->rfield, sizeof(field));
			childNode->lv = root->lv + 1;

			// 내려갈 수 있을만큼의 위치 확인
			rBlockY = 0;
			while(CheckToMove(childNode->rfield, nextBlock[root->lv], rBlockR, rBlockY, rBlockX) == 1) {
				rBlockY++;
			}
			rBlockY--;
			rScore = AddBlockToField(childNode->rfield, nextBlock[root->lv], rBlockR, rBlockY, rBlockX);
			rScore += DeleteLine(childNode->rfield);

			if(childNode->lv < VISIBLE_BLOCKS) {
				rScore += recommend(childNode);
			}

			if(result <= rScore) {
				if(root->lv == 0) {
					if(rScore > result || rBlockY > recommendY){
						recommendY = rBlockY;
						recommendX = rBlockX;
						recommendR = rBlockR;
					}
				}
				result = rScore;
			}

		}
	}
	return result;
}


int modified_recommend(RecNode* root){
	RecNode parentNode;
	RecNode* childNode = &parentNode;
	int rBlockX, rBlockY, rBlockR, rScore, result = -1;

	for(rBlockR = 0; rBlockR < 4; rBlockR++) {
		for(rBlockX = -2; rBlockX < WIDTH + 3; rBlockX++) {
			if(!CheckToMove(root->rfield, nextBlock[root->lv], rBlockR, 0, rBlockX)) {
				continue;
			}
			
			memcpy(childNode->rfield, root->rfield, sizeof(field));
			childNode->lv = root->lv + 1;

			// 내려갈 수 있을만큼의 위치 확인
			rBlockY = 0;
			while(CheckToMove(childNode->rfield, nextBlock[root->lv], rBlockR, rBlockY, rBlockX) == 1) {
				rBlockY++;
			}
			rBlockY--;
			rScore = AddBlockToField(childNode->rfield, nextBlock[root->lv], rBlockR, rBlockY, rBlockX);
			rScore += DeleteLine(childNode->rfield) * DELETE_WEIGHT;

			if(childNode->lv < VISIBLE_BLOCKS - 1) {
				rScore += modified_recommend(childNode);
			}
			else {
				rScore += evaluateWeight(childNode->rfield);
			}

			if(root->lv == 0) {
				int fullFlag = 0;
				for (int i = 0; i < MAX_BLOCKS; i++) {
					if(maxValue[i] == -1){
						maxValue[i] = rScore;
						maxPath[i][0] = rBlockY;
						maxPath[i][1] = rBlockX;
						maxPath[i][2] = rBlockR;
						fullFlag = 1;
						break;
					}
				}
				if (fullFlag == 0) {
					for (int j = MAX_BLOCKS - 1; j >= 0; j--) {
						if (maxValue[j] < rScore || (maxValue[j] == rScore && rBlockY > maxPath[j][0])) {
							if(j < MAX_BLOCKS - 1) {
								maxValue[j + 1] = maxValue[j];
								maxPath[j + 1][0] = maxPath[j][0];
								maxPath[j + 1][1] = maxPath[j][1];
								maxPath[j + 1][2] = maxPath[j][2];
							}
							maxValue[j] = rScore;
							maxPath[j][0] = rBlockY;
							maxPath[j][1] = rBlockX;
							maxPath[j][2] = rBlockR;
						}
						else{
							break;
						}
					}
				}
			
			}
			else if(result <= rScore) {
				result = rScore;
			}
		}
	}
	if(root->lv == 0){
		result = fastRecommend(root);
	}
	return result;
}

int fastRecommend(RecNode* root){
	RecNode parentNode;
	RecNode* childNode = &parentNode;
	int rBlockX, rBlockY, rBlockR, rScore, result = -1;

	if (root->lv == 0) {
		for (int i = MAX_BLOCKS - 1; i >= 0; i--) {
			memcpy(childNode->rfield, root->rfield, sizeof(field));
			childNode->lv = root->lv + 1;

			rBlockY = maxPath[i][0];
			rBlockX = maxPath[i][1];
			rBlockR = maxPath[i][2];

			rScore = AddBlockToField(childNode->rfield, nextBlock[root->lv], rBlockR, rBlockY, rBlockX);
			rScore += DeleteLine(childNode->rfield) * DELETE_WEIGHT;
			
			rScore += fastRecommend(childNode);

			if(result <= rScore) {
				if(rScore > result || rBlockY > recommendY){
					recommendY = rBlockY;
					recommendX = rBlockX;
					recommendR = rBlockR;
				}
				result = rScore;
			}
		}
	}
	else {
		for(rBlockR = 0; rBlockR < 4; rBlockR++) {
			for(rBlockX = -2; rBlockX < WIDTH + 3; rBlockX++) {
				if(!CheckToMove(root->rfield, nextBlock[root->lv], rBlockR, 0, rBlockX)) {
					continue;
				}

				memcpy(childNode->rfield, root->rfield, sizeof(field));
				childNode->lv = root->lv + 1;

				// 내려갈 수 있을만큼의 위치 확인
				rBlockY = 0;
				while(CheckToMove(childNode->rfield, nextBlock[root->lv], rBlockR, rBlockY, rBlockX) == 1) {
					rBlockY++;
				}
				rBlockY--;
				rScore = AddBlockToField(childNode->rfield, nextBlock[root->lv], rBlockR, rBlockY, rBlockX);
				rScore += DeleteLine(childNode->rfield) * DELETE_WEIGHT;

				if(childNode->lv < VISIBLE_BLOCKS) {
					rScore += fastRecommend(childNode);
				}
				else {
					rScore += evaluateWeight(childNode->rfield);
				}

				if(result <= rScore) {
					result = rScore;
				}
			}
		}
	}
	return result;
}

void recommendedPlay(){
	// user code
	recommendPlaying = 1;
    play();
	recommendPlaying = 0;
}


int evaluateWeight(char field[HEIGHT][WIDTH]) {
	int weight = 0, y, x, holeNum, holeFlag;
	for (x = 0; x < WIDTH; x++) {
		holeNum = 0;
		holeFlag = 0;
		for (y = 0; y < HEIGHT; y++) {
			if (field[y][x]) {
				weight += holeNum * HOLE_WEIGHT;
				holeNum = 0;
				holeFlag = 1;
			}
			else {
				holeNum += holeFlag;
			}
		}
	}
	return weight;
}
