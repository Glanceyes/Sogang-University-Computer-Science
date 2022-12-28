#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include "MyHeader.h"

CASES* testCases = NULL;
int testCaseNum = 0;
int currentCase = 0;

void F1(void){
	if(testCaseNum > 0) {
		for(int i = 0; i < testCaseNum; i++){
			free(testCases[i].nodeData);
		}
		free(testCases);
	}
	testCases = NULL;
	testCaseNum = 0;
	currentCase = 0;
}

void F2(const char *filename){
	FILE* fp;
	fp = fopen(filename, "r");

	fscanf(fp, "%d\n", &testCaseNum);
	testCases = (CASES*)malloc(sizeof(CASES) * testCaseNum);

	for(int i = 0; i < testCaseNum; i++){
		fscanf(fp, "%d\n", &testCases[i].coupleNum);
		testCases[i].nodeData = (NODEDATA*)malloc(sizeof(NODEDATA) * testCases[i].coupleNum);
		for(int j = 0; j < testCases[i].coupleNum; j++){
			if((j == testCases[i].coupleNum - 1) || (j % 5 == 4)){
				fscanf(fp, "%d %d\n", &testCases[i].nodeData[j].parent, &testCases[i].nodeData[j].child);
			}
			else{
				fscanf(fp, "%d %d  ", &testCases[i].nodeData[j].parent, &testCases[i].nodeData[j].child);
			}
		}
	}
	fclose(fp);
}

char* F3(int number ){
	int maxNode = 0;
	int zeroCount = 0;
	int notTreeFlag = 0;
	char buffer[100];	
	char *resultString = new char[256];
	sprintf(buffer, "%d", number);
	sprintf(resultString,"Case %d", number);

	number--;
	for(int i = 0; i < testCases[number].coupleNum; i++){
		if(maxNode < testCases[number].nodeData[i].parent){
			maxNode = testCases[number].nodeData[i].parent;
		}
		if(maxNode < testCases[number].nodeData[i].child){
			maxNode = testCases[number].nodeData[i].child;
		}
	}

	int* P = new int[maxNode + 1];
	
	for(int i = 0; i < maxNode + 1; i++) {
		P[i] = -1;
	}

	for(int i = 0; i < testCases[number].coupleNum; i++){
		if(P[testCases[number].nodeData[i].child] > 0) {
			notTreeFlag = 1;
		}
		P[testCases[number].nodeData[i].child] = testCases[number].nodeData[i].parent;
		if(P[testCases[number].nodeData[i].parent] == -1){
			P[testCases[number].nodeData[i].parent] = 0;
		}
	}
	for(int i = 0; i < maxNode + 1; i++){
		if(P[i] == 0) zeroCount++;
	}


	if(Cycle_Check(P, maxNode + 1) == true || notTreeFlag == 1 || zeroCount != 1){
		strcat(resultString, " is not a tree.");
	}
	else {
		strcat(resultString, " is a tree.");
	}

	delete[] P;
	return resultString;
}

bool dfs(int P[], int flag[], int c){
	flag[c] = 1;
	if(P[c] == 0) return false;
	else if(flag[P[c]] == 1) return true;
	else return dfs(P, flag, P[c]);
}

bool Cycle_Check (int P[], int NODE_MAX_NUM){
	int i, j;
	int *flag = new int[NODE_MAX_NUM];

	for(j = 0; j < NODE_MAX_NUM; j++){
		if (P[j] <= 0) continue;
		for (i = 0; i < NODE_MAX_NUM; i++)
			flag[i] = -1;
		if (dfs(P, flag, j) == true) {
			delete[] flag;
			return true;
		}
	}
	delete[] flag;
	return false;
}