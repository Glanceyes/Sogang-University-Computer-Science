#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Execute an O(n^2) algorithm.
void MaxSubSequenceSum1(int* Sequence, int SequenceLength, int* StartIndex, int* EndIndex, int* MaxSum) {
    
	// 'ThisSum' means the sum of the subsequence from ith element to jth element.
    int ThisSum;

    *MaxSum = Sequence[0];
    *StartIndex = *EndIndex = 0;
    for (int i = 0; i < SequenceLength; i++) {
        ThisSum = 0;
        for (int j = i; j < SequenceLength; j++) {
            ThisSum += Sequence[j];
			// If 'ThisSum' is greater than 'MaxSum',
            if (ThisSum > *MaxSum) {
                *MaxSum = ThisSum;
                *StartIndex = i;
                *EndIndex = j;
            }
			// Update a value of 'Maxsum' to 'ThisSum' and save the starting and the ending indexes of the subsequence.
        }
    }
    
    return;
};

// Execute an O(nlongn) algorithm.
void MaxSubSequenceSum2(int* Sequence, int* StartIndex, int* EndIndex, int* MaxSum, int Left, int Right) {

    int MaxLeftSum, MaxRightSum, MaxLeftBorderSum, MaxRightBorderSum, LeftBorderSum, RightBorderSum, Center, LeftSumStartIndex, LeftSumEndIndex, RightSumStartIndex, RightSumEndIndex, LeftBorderIndex, RightBorderIndex;

	// An termination condition in recursion. 
    if (Left == Right) {
        *MaxSum = Sequence[Left];
        *StartIndex = *EndIndex = Left;
        return;
    }



	// Find the middle index of the subsequence to divide it into two parts(left and right).
    Center = (Left + Right) / 2;

	// Call the recursive function that finds the maximum sum of a left part of the current sequence.
    MaxSubSequenceSum2(Sequence, StartIndex, EndIndex, MaxSum, Left, Center);
    MaxLeftSum = *MaxSum;
    LeftSumStartIndex = *StartIndex;
    LeftSumEndIndex = *EndIndex;

	// Call the recursive function that finds the maximum sum of a right part of the current sequence.
    MaxSubSequenceSum2(Sequence, StartIndex, EndIndex, MaxSum, Center + 1, Right);
    MaxRightSum = *MaxSum;
    RightSumStartIndex = *StartIndex;
    RightSumEndIndex = *EndIndex;



	// Need to know whether the subsequence that has maximum sum of the current sequence is crossing the 'Center'.
	// The sum of it will be the sum of 'MaxLeftBordersum' and 'MaxRightBorderSum'.
    MaxLeftBorderSum = Sequence[Center];
    LeftBorderSum = 0;
    LeftBorderIndex = Center;
	// Update 'MaxLeftBorderSum' to 'LeftBorderSum' when the subsequence from ith to 'Center'th element has the maximum sum.
    for (int i = Center; i >= Left; i--) {
        LeftBorderSum += Sequence[i];
        if (LeftBorderSum >= MaxLeftBorderSum) {
            MaxLeftBorderSum = LeftBorderSum;
            LeftBorderIndex = i;
        }
    }

    MaxRightBorderSum = Sequence[Center + 1];
    RightBorderSum = 0;
    RightBorderIndex = Center + 1;
	// Update 'MaxRightBorderSum' to 'RightBorderSum' when the subsequence from 'Center + 1'th to ith element has the maximum sum.
    for (int i = Center + 1; i <= Right; i++) {
        RightBorderSum += Sequence[i];
        if (RightBorderSum > MaxRightBorderSum) {
            MaxRightBorderSum = RightBorderSum;
            RightBorderIndex = i;
        }
    }



	// Find the maximum in three values, 'MaxLeftSum', 'MaxRightSum', and 'MaxLeftBorderSum' + 'MaxRightBorderSum'.
    *MaxSum = MaxLeftSum;
    *StartIndex = LeftSumStartIndex;
    *EndIndex = LeftSumEndIndex;
    if (*MaxSum < MaxRightSum) {
        *MaxSum = MaxRightSum;
        *StartIndex = RightSumStartIndex;
        *EndIndex = RightSumEndIndex;
    }
    if (*MaxSum < MaxLeftBorderSum + MaxRightBorderSum) {
        *MaxSum = MaxLeftBorderSum + MaxRightBorderSum;
        *StartIndex = LeftBorderIndex;
        *EndIndex = RightBorderIndex;
    }
	else if (*MaxSum == MaxLeftBorderSum + MaxRightBorderSum) {
		if (*StartIndex >= LeftBorderIndex && *EndIndex >= RightBorderIndex) {
			*StartIndex = LeftBorderIndex;
			*EndIndex = RightBorderIndex;
		}
	}

    return;
};

// Execute an O(n) algorithm.
void MaxSubSequenceSum3(int* Sequence, int SequenceLength, int* StartIndex, int* EndIndex, int* MaxSum) {

	// 'ThisSum' means the maximum sum of subsequence in the current sequence to ith index.
    int ThisSum, TempStartIndex = 0;

    *MaxSum = Sequence[0];
    ThisSum = 0;
    *StartIndex = *EndIndex = 0;
    for (int i = 0; i < SequenceLength; i++) {
        ThisSum += Sequence[i];
		// If 'ThisSum' is greater than 'MaxSum',
        if (ThisSum > *MaxSum) {
            *MaxSum = ThisSum;
			*StartIndex = TempStartIndex;
            *EndIndex = i;
        }
		// Update a value of 'Maxsum' to 'ThisSum' and save the ending index of the subsequence.
		// If not and 'ThisSum' is negative, then the sum will decrease.
        else if (ThisSum < 0) {
            ThisSum = 0;
            TempStartIndex = i + 1;
        }
		// Thus, initialize 'ThisSum' and 'TempStartIndex' to find the maximum sum of the subsequence that excludes a previous one.
    }

    return;
};

// 'argc' and 'argv' contain the number of arguments and the values of arguments respectively.
int main(int argc, char **argv) {
    int SequenceLength, AlgorithmIndex, StartIndex = 0, EndIndex = 0, MaxSum, *Sequence = NULL, NumberBuffer;
    clock_t StartRunningTime, EndRunningTime;

	// Check if the number of arguments is 2. 
	//The another argument is a command and included in 'argc', so we need to check if a value of 'argc' is 3.
    if (argc != 3){
    	printf("The Number of arguments is larger or smaller. You need 2 arguments.");
	    return 0;
    }

	// 'argv[1]' contains a file name that we need to read.
    char *FileName = (char*)malloc((int)(strlen(argv[1])+1));
    snprintf(FileName, (int)(strlen(argv[1]) + 1), "%s", argv[1]);

	// 'argv[2]' contains an index of an algorithm that we execute.  
    AlgorithmIndex = atoi(argv[2]);



	// Read an input file.
    FILE *FilePointer = fopen(FileName, "r");
	// Check if an error occurs while reading the file.
    if (FilePointer == NULL) {
        printf("File Error!\n");
        free(FileName);
        return 0;
    }

	// Read the first number in the file that indicates the number of elements.
	// Check if an error occurs while scanning the file.
    if (fscanf(FilePointer, "%d ", &SequenceLength) == EOF) {
        printf("File Scan Error!\n");
        fclose(FilePointer);
        free(FileName);
        return 0;
    }

	// Allocate the size of memory at 'Sequence' as much as the number of elements.
    Sequence = (int*)malloc(sizeof(int) * SequenceLength);

	// Read the elements of sequence.
    for (int i = 0; i < SequenceLength; i++) {
        if (EOF == fscanf(FilePointer, "%d ", &NumberBuffer)) break;
        Sequence[i] = NumberBuffer;
    }

	// Close the input file.
    fclose(FilePointer);



	// Start to measure the milliseconds by using a function 'clock'.
    StartRunningTime = clock();

	// Determine which algorithm should be execute by checking the index of algorithm. 
    switch (AlgorithmIndex) {
        case 1: MaxSubSequenceSum1(Sequence, SequenceLength, &StartIndex, &EndIndex, &MaxSum);
            break;
        case 2: MaxSubSequenceSum2(Sequence, &StartIndex, &EndIndex, &MaxSum, 0, SequenceLength - 1);
            break;
        case 3: MaxSubSequenceSum3(Sequence, SequenceLength, &StartIndex, &EndIndex, &MaxSum);
            break;
		default: printf("The index of an algorithm is wrong!\n");
			free(FileName);
			free(Sequence);
			return 0;
    }

	// End the measuring time.
    EndRunningTime = clock();
    


	// Make a name of an output file.
    char* ResultFileName = (char*)malloc((int)strlen(FileName) + sizeof(char) * 7 + 1);
    snprintf(ResultFileName, (int)strlen(FileName) + sizeof(char)*7 + 1, "result_%s", FileName);

	// Produce the output file.
    FILE *OutputFilePointer = fopen(ResultFileName, "w");
	// Check if an error occurs while writing the file.
    if (OutputFilePointer == NULL) {
        printf("File Write Error!\n");
        free(FileName);
        free(ResultFileName);
        free(Sequence);
        return 0;
    }
    
	// Write the file containing the information of an algorithm and the result.
    fprintf(OutputFilePointer, "%s\n%d\n%d\n%d\n%d\n%d\n%.5lf\n", FileName, AlgorithmIndex, SequenceLength, StartIndex, EndIndex, MaxSum, 1000 * ((double)(EndRunningTime - StartRunningTime) / CLOCKS_PER_SEC));



	// Close the output file.
    fclose(OutputFilePointer);

	// Free the memories that is allocated dynamically. 
    free(FileName);
    free(ResultFileName);
    free(Sequence);
    return 0;
}
