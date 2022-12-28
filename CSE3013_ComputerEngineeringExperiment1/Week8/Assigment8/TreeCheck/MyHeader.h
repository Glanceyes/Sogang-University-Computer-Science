#include "stdafx.h"

typedef struct {
	int parent, child;
}NODEDATA;

typedef struct cases{
	int coupleNum;
	NODEDATA* nodeData;
}CASES;

extern int testCaseNum, currentCase;
extern CASES* testCases;
extern char *resultString;

void F1(void);
void F2(const char *filename);
char* F3(int number);
bool dfs(int P[], int flag[], int c);
bool Cycle_Check(int P[], int NODE_MAX_NUM);