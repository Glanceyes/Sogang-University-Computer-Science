#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int PageTotal, PageCount[10];
} PageCountList;

void DoCountPage (PageCountList *PageInput);
void PrintCountPage (PageCountList PageInput);