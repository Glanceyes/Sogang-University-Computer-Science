#include "Header.h"

void DoCountPage (PageCountList *PageInput) {
	int TempNum, TempNumDigit;

	for (int i = 0; i < 10; i++) {
		PageInput->PageCount[i] = 0;
	}

	for (int i = 1; i <= PageInput->PageTotal; i++) {
		TempNum = i;
		while (TempNum) {
			(PageInput->PageCount[TempNum % 10])++;
			TempNum /= 10;
		}
	}

	return;
}
