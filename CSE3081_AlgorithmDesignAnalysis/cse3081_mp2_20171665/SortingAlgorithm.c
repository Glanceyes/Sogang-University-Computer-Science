#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

int GetNumberOfDigits(int Number) {
    int DigitCounter = 0;
    if (Number == 0) {
        return 1;
    }
    while (Number) {
        Number = Number / 10;
        DigitCounter++;
    }
    return DigitCounter;
}

int GetNumberOfDepth(int Number) {
    int DigitCounter = 0;
    if (Number == 0) {
        return 1;
    }
    while (Number) {
        Number = Number / 2;
        DigitCounter++;
    }
    return DigitCounter;
}

// Execute an insertion sorting algorithm.
void InsertionSortAlgorithm (int *List, int ListLength) {
    int i, j, Temp;

    for (i = 1; i < ListLength; i++) {
        Temp = List[i];
        j = i;
        while ((j > 0) && (Temp < List[j - 1])) {
            List[j] = List[j - 1];
            j--;
        }
        List[j] = Temp;
    }
    return;
}

int QuickSortPartition(int* List, int LeftIndex, int RightIndex) {
    int Pivot = LeftIndex, Temp;
    for (int i = LeftIndex; i < RightIndex; i++) {
        if (List[i] < List[RightIndex]) {
            Temp = List[i];
            List[i] = List[Pivot];
            List[Pivot++] = Temp;
        }
    }
    Temp = List[RightIndex];
    List[RightIndex] = List[Pivot];
    List[Pivot] = Temp;

    return Pivot;
}

// Execute an quick sorting algorithm.
void QuickSortAlgorithm(int* List, int LeftIndex, int RightIndex) {
    int* QuickSortStack, StackTop = -1;
    QuickSortStack = (int*)malloc(sizeof(int)*(RightIndex - LeftIndex + 1));
    
    QuickSortStack[++StackTop] = LeftIndex;
    QuickSortStack[++StackTop] = RightIndex;

    while (StackTop >= 0) {
        RightIndex = QuickSortStack[StackTop--];
        LeftIndex = QuickSortStack[StackTop--];

        int Pivot = QuickSortPartition(List, LeftIndex, RightIndex);

        if (Pivot - 1 > LeftIndex) {
            QuickSortStack[++StackTop] = LeftIndex;
            QuickSortStack[++StackTop] = Pivot - 1;
        }

        if (Pivot + 1 < RightIndex) {
            QuickSortStack[++StackTop] = Pivot + 1;
            QuickSortStack[++StackTop] = RightIndex;
        }
    }
    return;
}

// Execute an merge sorting algorithm.
void MergeSortAlgorithm (int *List, int LeftIndex, int RightIndex) {
    int MiddleIndex;

    if (LeftIndex < RightIndex) {
        MiddleIndex = (LeftIndex + RightIndex) / 2;
        
        MergeSortAlgorithm(List, LeftIndex, MiddleIndex);
        MergeSortAlgorithm(List, MiddleIndex + 1, RightIndex);

        int* Buffer = NULL;
        int TempLeftIndex, TempRightIndex, TempIndex;
        Buffer = (int*)malloc(sizeof(int) * (RightIndex - LeftIndex + 1));
        
        for (int i = 0; i < RightIndex - LeftIndex + 1; i++) {
            Buffer[i] = List[LeftIndex + i];
        }

        TempLeftIndex = 0;
        TempRightIndex = MiddleIndex - LeftIndex + 1;
        TempIndex = LeftIndex;

        while ((TempLeftIndex <= MiddleIndex - LeftIndex) && (TempRightIndex <= RightIndex - LeftIndex)) {
            if (Buffer[TempLeftIndex] < Buffer[TempRightIndex]) {
                List[TempIndex++] = Buffer[TempLeftIndex++];
            }
            else {
                List[TempIndex++] = Buffer[TempRightIndex++];
            }
        }

        while (TempLeftIndex <= MiddleIndex - LeftIndex) {
            List[TempIndex++] = Buffer[TempLeftIndex++];
        }
        while (TempRightIndex <= RightIndex - LeftIndex) {
            List[TempIndex++] = Buffer[TempRightIndex++];
        }
        free(Buffer);
    }
    return;
}

// Sub-functions for executing a radix-introspective sorting algorithm.
int IsSorted(int *List, int Last) {
    for (int i = 0; i < Last; i++) {
        if (List[i] > List[i + 1]) {
            return 0;
        }
        else {
            return 1;
        }
    }
    return 1;
}

void AdjustHeap(int *List, int Begin, int First, int Size){
    int Temp = List[First + Begin - 1], Child;
    while (Begin <= Size / 2) {
        Child = 2 * Begin;
        if (Child < Size && List[First + Child - 1] < List[First + Child]) {
            Child++;
        }
        if (Temp > List[First + Child - 1]) {
            break;
        }
        List[First + Begin - 1] = List[First + Child - 1];
        Begin = Child;
    }
    List[First + Begin - 1] = Temp;
}

void HeapSort(int *List, int First, int Last) {
    int Length = Last - First;

    for (int i = Length / 2; i >= 1; i--) {
        AdjustHeap(List, i, First, Length);
    }
    for (int i = Length; i > 1; i--) {
        int Temp = List[First];
        List[First] = List[First + i - 1];
        List[First + i - 1] = Temp;
        AdjustHeap(List, 1, First, i - 1);
    }
}

int MedianOfThree(int *List, int LeftIndex, int MiddleIndex, int RightIndex) {
    if (List[MiddleIndex] < List[LeftIndex]){
        if (List[RightIndex] < List[MiddleIndex]) {
            return List[MiddleIndex];
        }
        else {
            if (List[RightIndex] < List[LeftIndex]) {
                return List[RightIndex];
            }
            else {
                return List[LeftIndex];
            }
        }
    }
    else {
        if (List[RightIndex] < List[MiddleIndex]) {
            if (List[RightIndex] < List[LeftIndex]) {
                return List[LeftIndex];
            }
            else {
                return List[RightIndex];
            }
        }
        else {
            return List[MiddleIndex];
        }
    }
}

int Partition(int *List, int LeftIndex, int RightIndex, int Median) {
    int Low = LeftIndex, High = RightIndex, Temp;
    while(1) {
        while (List[Low] < Median) {
            Low++;
        }
        High--;
        while (Median < List[High]) {
            High--;
        }
        if (Low >= High) {
            return Low;
        }
        Temp = List[Low];
        List[Low] = List[High];
        List[High] = Temp;
        Low++;
    }
}

// Customization of quick sort
void IntroSortLoop(int *List, int LeftIndex, int RightIndex, int Depth){
    while (RightIndex - LeftIndex > 20) {
        if (Depth == 0) {
            HeapSort(List, LeftIndex, RightIndex);
            return;
        }
        Depth = Depth - 1;
        int Pivot = Partition(List, LeftIndex, RightIndex, MedianOfThree(List, LeftIndex, LeftIndex + (RightIndex - LeftIndex) / 2 + 1, RightIndex - 1));
        IntroSortLoop(List, Pivot, RightIndex, Depth);
        RightIndex = Pivot;
    }
    return;
}

void IntroSortAlgorithm(int *List, int LeftIndex, int RightIndex, int ListLength) {
    IntroSortLoop(List, LeftIndex, RightIndex, ((int)(2 * (GetNumberOfDepth(ListLength) - 1))));
    InsertionSortAlgorithm(List, ListLength);

    return;
}

// Execute a radix-introspective sorting algorithm.
void RadixIntroSortAlgorithm(int* List, int LeftIndex, int RightIndex, int ListLength) {
    int i, MaxValue = 0, AbsMaxValue = 0, DigitValue[10], *SortedList = NULL, *NegativeNumber = NULL, *NonNegativeNumber = NULL, NegativeNumCounter = 0, NonNegativeNumCounter = 0;
    int SignDigit = 1, TempSignDigit, NegativeSignDigit = 1, NonNegativeSignDigit = 1, NegativeOverFlowFlag = 0, NonNegativeOverFlowFlag = 0;

    NegativeNumber = (int*)malloc(sizeof(int) * ListLength);
    NonNegativeNumber = (int*)malloc(sizeof(int) * ListLength);

    for (i = 0; i < ListLength; i++) {
        if (List[i] < 0) {
            NegativeNumber[NegativeNumCounter++] = List[i] * (-1);
            if (List[i] < (-1)*AbsMaxValue) {
                AbsMaxValue = List[i] * (-1);
                while (AbsMaxValue / NegativeSignDigit > 0) {
                    if (NegativeSignDigit >= 1000000000) {
                        NegativeOverFlowFlag = 1;
                        break;
                    }
                    NegativeSignDigit *= 10;
                }
            }
        }
        else {
            NonNegativeNumber[NonNegativeNumCounter++] = List[i];
            if (List[i] > MaxValue) {
                MaxValue = List[i];
                while (MaxValue / NonNegativeSignDigit > 0) {
                    if (NonNegativeSignDigit >= 1000000000) {
                        NonNegativeOverFlowFlag = 1;
                        break;
                    }
                    NonNegativeSignDigit *= 10;
                }
            }
        }
    }

    if (NegativeOverFlowFlag == 0) {
        NegativeSignDigit = NegativeSignDigit / 10;
    }

    if (NonNegativeOverFlowFlag == 0) {
        NonNegativeSignDigit = NonNegativeSignDigit / 10;
    }

    if (NegativeSignDigit > NonNegativeSignDigit) {
        TempSignDigit = NegativeSignDigit;
    }
    else {
        TempSignDigit = NonNegativeSignDigit;
    }

    if (GetNumberOfDigits(TempSignDigit) > GetNumberOfDigits(ListLength) - 1) {
        IntroSortAlgorithm(List, LeftIndex, RightIndex, ListLength);
    }
    else {
        if (NegativeNumCounter > 0) {
            SortedList = (int*)malloc(sizeof(int) * NegativeNumCounter);
            NegativeNumber = realloc(NegativeNumber, sizeof(int) * NegativeNumCounter);
            while (AbsMaxValue / SignDigit > 0) {
                for (i = 0; i < 10; i++) DigitValue[i] = 0;
                for (i = 0; i < NegativeNumCounter; i++) {
                    DigitValue[(9 - (NegativeNumber[i] / SignDigit) % 10)]++;
                }
                for (i = 1; i < 10; i++) {
                    DigitValue[i] += DigitValue[i - 1];
                }
                for (i = NegativeNumCounter - 1; i >= 0; i--) {
                    SortedList[--DigitValue[(9 - (NegativeNumber[i] / SignDigit) % 10)]] = NegativeNumber[i];
                }

                for (i = 0; i < NegativeNumCounter; i++) {
                    NegativeNumber[i] = SortedList[i];
                }
                if (SignDigit * 10 > 1000000000) {
                    break;
                }
                SignDigit *= 10;
            }
            for (i = 0; i < NegativeNumCounter; i++) {
                List[i] = NegativeNumber[i] * (-1);
            }
            
            
            free(SortedList);
            SignDigit = 1;
        }

        if (NonNegativeNumCounter > 0) {
            SortedList = (int*)malloc(sizeof(int) * NonNegativeNumCounter);
            NonNegativeNumber = realloc(NonNegativeNumber, sizeof(int) * NonNegativeNumCounter);
            while (MaxValue / SignDigit > 0) {
                for (i = 0; i < 10; i++) DigitValue[i] = 0;
                for (i = 0; i < NonNegativeNumCounter; i++) {
                    DigitValue[(NonNegativeNumber[i] / SignDigit) % 10]++;
                }
                for (i = 1; i < 10; i++) {
                    DigitValue[i] += DigitValue[i - 1];
                }

                for (i = NonNegativeNumCounter - 1; i >= 0; i--) {
                    SortedList[--DigitValue[(NonNegativeNumber[i] / SignDigit) % 10]] = NonNegativeNumber[i];
                }

                for (i = 0; i < NonNegativeNumCounter; i++) {
                    NonNegativeNumber[i] = SortedList[i];
                }

                if (SignDigit * 10 > 1000000000) {
                    break;
                }

                SignDigit *= 10;
            }
            for (i = 0; i < NonNegativeNumCounter; i++) {
                List[i + NegativeNumCounter] = NonNegativeNumber[i];
            }

            free(SortedList);
        }
    }

    free(NegativeNumber);
    free(NonNegativeNumber);
    return;
}

// 'argc' and 'argv' contain the number of arguments and the values of arguments respectively.
int main(int argc, char **argv) {
    int AlgorithmIndex, ListLength, *List = NULL, NumberBuffer;
    clock_t StartRunningTime, EndRunningTime;

    // Check if the number of arguments is 2. 
     //The another argument is a command and included in 'argc', so we need to check if a value of 'argc' is 3.
    if (argc != 3) {
        printf("The Number of arguments is larger or smaller. You need 2 arguments.");
        return 0;
    }

    // 'argv[1]' contains a file name that we need to read.
    char *FileName = (char*)malloc((int)(strlen(argv[1]) + 1));
    snprintf(FileName, (int)(strlen(argv[1]) + 1), "%s", argv[1]);

    // 'argv[2]' contains an index of an algorithm that we execute.  
    AlgorithmIndex = atoi(argv[2]);



    // Read an input file.
    FILE *FilePointer = fopen(FileName, "r");
    // Check if an error occurs while reading the file.
    if (FilePointer == NULL) {
        printf("File Read Error!\n");
        free(FileName);
        return 0;
    }

    // Read the first number in the file that indicates the number of elements.
    // Check if an error occurs while scanning the file.
    if (fscanf(FilePointer, "%d ", &ListLength) == EOF) {
        printf("File Scan Error!\n");
        fclose(FilePointer);
        free(FileName);
        return 0;
    }

    // Allocate the size of memory at 'List' as much as the number of elements.
    List = (int*)malloc(sizeof(int) * ListLength);

    // Read the elements of list.
    for (int i = 0; i < ListLength; i++) {
        if (EOF == fscanf(FilePointer, "%d ", &NumberBuffer)) break;
        List[i] = NumberBuffer;
    }

    fclose(FilePointer);


    StartRunningTime = clock();

    switch (AlgorithmIndex) {
        case 1: InsertionSortAlgorithm(List, ListLength);
            break;
        case 2: QuickSortAlgorithm(List, 0, ListLength - 1);
            break;
        case 3: MergeSortAlgorithm(List, 0, ListLength - 1);
            break;
        case 4: RadixIntroSortAlgorithm(List, 0, ListLength - 1, ListLength);
            break;
        default: printf("The index of an algorithm is wrong!\n");
            free(FileName);
            free(List);
            return 0;
    }

    EndRunningTime = clock();

    // Make a name of an output file.
    char* ResultFileName = (char*)malloc((int)strlen(FileName) + GetNumberOfDigits(AlgorithmIndex) + sizeof(char) * 8 + 1);
    snprintf(ResultFileName, (int)strlen(FileName) + GetNumberOfDigits(AlgorithmIndex) + sizeof(char) * 8 + 1, "result_%d_%s", AlgorithmIndex, FileName);

    // Produce the output file.
    FILE *OutputFilePointer = fopen(ResultFileName, "w");
    // Check if an error occurs while writing the file.
    if (OutputFilePointer == NULL) {
        printf("File Write Error!\n");
        free(FileName);
        free(ResultFileName);
        free(List);
        return 0;
    }


    // Write the file containing the information of an algorithm and the result.
    fprintf(OutputFilePointer, "%s\n%d\n%d\n%.5lf\n", FileName, AlgorithmIndex, ListLength, 1000 * ((double)(EndRunningTime - StartRunningTime) / CLOCKS_PER_SEC));
    for (int i = 0; i < ListLength; i++) {
        fprintf(OutputFilePointer, "%d ", List[i]);
    }
    fprintf(OutputFilePointer, "\n");



    // Close the output file.
    fclose(OutputFilePointer);

    // Free the memories that is allocated dynamically. 
    free(FileName);
    free(ResultFileName);
    free(List);
    return 0;
}