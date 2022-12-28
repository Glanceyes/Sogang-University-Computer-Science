#include "Header.h"

void PrintCountPage (PageCountList PageInput) {
	for (int i = 0; i < 10; i++) {
		printf("%d ", PageInput.PageCount[i]);
	}
	printf("\n");
	return;
}