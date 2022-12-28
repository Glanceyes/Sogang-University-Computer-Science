#include "Header.h"

int main(void) {
	int InputNumber;
	PageCountList *PageInput = NULL;

	if(!scanf("%d", &InputNumber)) {
		printf("Failed to Read Integer!\n");
	}
	if (!InputNumber) {
		printf("The Number of Input Error!\n");
		return 0;
	}

	PageInput = (PageCountList*)malloc(sizeof(PageCountList) * InputNumber);
	
	for (int i = 0; i < InputNumber; i++){
		if(!scanf("%d", &(PageInput[i].PageTotal))) {
			printf("Failed to Read Integer!\n");
		}
	}

	for (int i = 0; i < InputNumber; i++){
		DoCountPage(&PageInput[i]);
	}

	for (int i = 0; i < InputNumber; i++){
		PrintCountPage(PageInput[i]);
	}

	free(PageInput);
	return 0;
}
