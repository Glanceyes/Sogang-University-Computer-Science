#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _NODE {
    char Symbol; // 캐릭터 심볼
    char *Bits; // 0과 1로 된 코드
    int Frequency; // 캐릭터 출현 빈도 수
    struct _NODE *Link; // Priority Queue를 구성하는 Linked List의 현재 노드에서 다음 노드를 가리킴
    struct _NODE *Left; // Huffman Tree에서 Left 노드 가리킴
    struct _NODE *Right; // Huffman Tree에서 Right 노드 가리킴
} NODE;


char *TempBits = NULL; 
int TotalBitNum = 0, LocationTotalBitNum = 0; // TotalBitNum: 압축된 파일의 전체 비트 수 / LocationTotalBitNum: 파일에서 TotalBitNum을 저장할 위치 
NODE *QueueFirst = NULL, *TreeRoot = NULL, *TempRoot = NULL;


/*-------------------------------------------------------
구현이 잘 되는지 확인하기 위한 테스트 용도
int CountQueue() {
    NODE* Current = QueueFirst;
    int Counter = 0;

    if (Current == NULL) return Counter;

    while (Current != NULL) {
        printf("%c(%d) ", Current->Symbol, Current->Frequency);
        if (!(Current->Left == NULL && Current->Right == NULL)) {
            printf("Left %c(%d) , Right %c(%d) ", Current->Left->Symbol, Current->Left->Frequency, Current->Right->Symbol, Current->Right->Frequency);
        }
        printf(" | ");
        Current = Current->Link;
        Counter++;
    }
    printf("\n");
    return Counter;
}

void InorderTest(NODE *Root) {
    if (Root != NULL) {
        InorderTest(Root->Left);
        if (Root->Left == NULL && Root->Right == NULL && Root->Symbol == '\0') {
            printf("Tree : Symbol: %c, Bits: %s, Frequency: %d\n", Root->Symbol, Root->Bits, Root->Frequency);
        }
        InorderTest(Root->Right);
    }
}
-------------------------------------------------------*/


// 노드 생성
NODE* MakeNode(char TempChar) {
    NODE *NewNode = (NODE*)malloc(sizeof(NODE));
    NewNode->Symbol = TempChar;
    NewNode->Frequency = 1;
    NewNode->Bits = NULL;
    NewNode->Link = NULL;
    NewNode->Left = NULL;
    NewNode->Right = NULL;

    return NewNode;
}

// Priority Queue에 우선순위의 위치에 맞게 노드 삽입
void QueueInsert(NODE* AddNode) {
    NODE *TempNode = QueueFirst, *Previous = NULL;
    if (QueueFirst == NULL) {
        QueueFirst = AddNode;
        return;
    }

    if (TempNode->Frequency > AddNode->Frequency) {
        AddNode->Link = TempNode;
        QueueFirst = AddNode;
    }
    else {
        Previous = TempNode;
        while (AddNode->Frequency > TempNode->Frequency) {
            Previous = TempNode;
            TempNode = TempNode->Link;
            if (TempNode == NULL) {
                Previous->Link = AddNode;
                return;
            }
        }
        AddNode->Link = Previous->Link;
        Previous->Link = AddNode;
    }

    return;
}

// Priority Queue에서 우선순위가 가장 높은 노드인 맨 앞 노드 뽑기
NODE* QueueDelete() {
    NODE *Current = QueueFirst;

    if (QueueFirst == NULL) {
        return NULL;
    }

    QueueFirst = QueueFirst->Link;
    Current->Link = NULL;

    return Current;
}

// Priority Queue 생성
void MakePriorityQueue(char TempChar, int* CharNum) {
    NODE *Current = NULL, *Previous = NULL;

    if (QueueFirst == NULL) { // Priority Queue가 생성이 아예 안 되었으면 처음 노드 생성
        QueueFirst = MakeNode(TempChar);
        (*CharNum)++;
        return;
    }

    Current = QueueFirst;

    if (Current->Symbol == TempChar) {
        Current->Frequency = Current->Frequency + 1;
        if (Current->Link == NULL) {
            return;
        }
        if (Current->Frequency > Current->Link->Frequency) {
            QueueFirst = Current->Link;
            Current->Link = NULL;
            QueueInsert(Current);
        }
        return;
    }

    while (Current->Link != NULL) {
        if (Current->Symbol == TempChar) {
            break;
        }
        Previous = Current;
        Current = Current->Link;
    }

    if (Current->Symbol == TempChar) {
        Current->Frequency = Current->Frequency + 1;
        if (Current->Link == NULL) {
            return;
        }
        if (Current->Frequency > Current->Link->Frequency) {
            Previous->Link = Current->Link;
            Current->Link = NULL;
            QueueInsert(Current);
        }
        return;
    }
    else if (Current->Link == NULL) {
        NODE* TempNode = MakeNode(TempChar);
        TempNode->Link = QueueFirst;
        QueueFirst = TempNode;
        (*CharNum)++;
        return;
    }
}

// Huffman Tree 생성
NODE* MakeTree(int CharNum) {
    NODE *NodeU = NULL, *NodeV = NULL, *NodeW = NULL;

    for (int i = 1; i <= CharNum - 1; i++) {
        NodeU = QueueDelete();
        

        /*-------------------------------------------------------
        테스트 용도
        printf("(symbol: %c / frequency: %d  ", NodeU->Symbol, NodeU->Frequency);
        if (!(NodeU->Left == NULL && NodeU->Right == NULL)) {
            printf("Left %c(%d) , Right %c(%d) ", NodeU->Left->Symbol, NodeU->Left->Frequency, NodeU->Right->Symbol, NodeU->Right->Frequency);
        }
        printf(")  ");
        -------------------------------------------------------*/ 


        NodeV = QueueDelete();
        

        /*-------------------------------------------------------
        테스트 용도
        printf("(symbol: %c / frequency: %d ", NodeV->Symbol, NodeV->Frequency);
        if (!(NodeV->Left == NULL && NodeV->Right == NULL)) {
            printf("Left %c(%d) , Right %c(%d) ", NodeV->Left->Symbol, NodeV->Left->Frequency, NodeV->Right->Symbol, NodeV->Right->Frequency);
        }
        printf(")  ");
        printf("\n");
        -------------------------------------------------------*/


        NodeW = MakeNode('\0'); // 부모 노드 생성
        NodeW->Left = NodeU;
        NodeW->Right = NodeV;
        NodeW->Frequency = NodeU->Frequency + NodeV->Frequency;
        QueueInsert(NodeW);


        /*-------------------------------------------------------
        테스트 용도
        CountQueue();
        -------------------------------------------------------*/
    }
    NodeW = QueueDelete();
    return NodeW; // root 노드를 반환해야 함 
}

// 0과 1로 된 코드 생성
void GenerateCode(NODE* Root, char* Bits) {
    char *LeftBits = NULL, *RightBits = NULL;

    if (Root != NULL) {
        if (!(Root->Left == NULL && Root->Right == NULL)) {
            LeftBits = (char*)malloc(strlen(Bits) + 2);
            RightBits = (char*)malloc(strlen(Bits) + 2);
            sprintf(LeftBits, "%s0", Bits);
            sprintf(RightBits, "%s1", Bits);
            Root->Left->Bits = LeftBits; // 노드에 Bits 정보를 담아야 함
            Root->Right->Bits = RightBits; // Bits는 포인터이니 그대로 대입
        }

        GenerateCode(Root->Left, LeftBits);
        GenerateCode(Root->Right, RightBits);
    }
}

// InOrder로 Huffman Tree 탐색하여 Symbol에 맞는 노드 찾기
void InorderTraverse(NODE *Root, char Symbol) {
    if (Root != NULL) {
        InorderTraverse(Root->Left, Symbol);
        if (Symbol != '\0') {
            if (Root->Symbol == Symbol) {
                TempBits = (char*)malloc(sizeof(char) * strlen(Root->Bits) + 1);
                sprintf(TempBits, "%s", Root->Bits);
                return;
            }
        }
        else {
            if (Root->Symbol == '\0' && Root->Left == NULL && Root->Right == NULL) {
                TempBits = (char*)malloc(sizeof(char) * strlen(Root->Bits) + 1);
                sprintf(TempBits, "%s", Root->Bits);
                return;
            }
        }
        InorderTraverse(Root->Right, Symbol);
    }
}

// Huffman Tree 메모리 할당 해제
void FreeTree(NODE* Root) {
    if (Root != NULL) {
        FreeTree(Root->Right);
        FreeTree(Root->Left);
        free(Root);
    }
}

// Priority Queue 메모리 할당 해제
void FreeQueue() {
    NODE *Previous = NULL, *Current = QueueFirst;

    if (QueueFirst->Link == NULL) {
        free(QueueFirst);
    }
    else {
        while (Current->Link == NULL) {
            Previous = Current;
            Current = Current->Link;
            free(Previous);
        }
        free(Current);
    }

    return;
}

// Header 영역에 저장하기 위한 Huffman Tree 데이터
void GenerateTreeData(FILE* FilePointer, NODE *Root, int *Counter, int FindNum) {
    if (Root != NULL) {
        GenerateTreeData(FilePointer, Root->Left, Counter, FindNum);
        if (Root->Left == NULL && Root->Right == NULL) {
            (*Counter)++;
            if ((*Counter) == FindNum) {
                char WriteBuffer[100];
                WriteBuffer[0] = Root->Symbol;
                WriteBuffer[1] = (char)strlen(Root->Bits);
                strcpy(&WriteBuffer[2], Root->Bits);


                /*-------------------------------------------------------
                테스트 용도
                //printf("Symbol: %c | Bits: %s | Length: %d\n", Root->Symbol, (WriteBuffer + 2), (int)strlen(WriteBuffer + 2));
                //if (Root->Symbol == '\0') {
                //   printf("%s\n", Root->Bits);
                //}
                -------------------------------------------------------*/


                fwrite(WriteBuffer, sizeof(char), 2 + strlen(Root->Bits), FilePointer);
            }
        }
        GenerateTreeData(FilePointer, Root->Right, Counter, FindNum);
    }
}

// 프로그램은 최소 1바이트 크기까지만 기록 가능하므로 8개의 비트를 1 바이트에 모두 채워서 파일에 작성
void WriteBit(FILE* FilePointer, int Bit, int CompleteFlag) {
    static char Byte = 0;
    static int BitNum = 0;
    char TempChar;
    if (CompleteFlag == 0) {
        if (Bit == 1) {
            TempChar = 1;
        }
        else if (Bit == 0) {
            TempChar = 0;
        }
        TempChar = TempChar << (7 - BitNum);
        Byte = Byte | TempChar;

        BitNum++;
        TotalBitNum++;

        if (BitNum == 8) {
            fwrite(&Byte, sizeof(char), 1, FilePointer);
            BitNum = 0;
            Byte = 0;
        }
    }
    else if (CompleteFlag == 1) {
        if (BitNum > 0) {
            fwrite(&Byte, sizeof(char), 1, FilePointer);
            BitNum = 0;
            Byte = 0;
        }
    }

}

// Huffman Tree의 Symbol에 맞는 노드의 코드를 파일에 작성
void MakeCompression(FILE* FilePointer) {
    int Counter = 0;

    while (TempBits[Counter] != '\0') {
        if (TempBits[Counter] == '1') {
            WriteBit(FilePointer, 1, 0);
        }
        else if (TempBits[Counter] == '0') {
            WriteBit(FilePointer, 0, 0);
        }
        Counter++;
    }

    if (TempBits != NULL) {
        free(TempBits);
    }
    return;
}

// 압축한 파일의 Header 영역 읽어서 Huffman Tree 복원
void RecoverTree(NODE* Root, char* ReadBuffer, char ReadSymbol, int BitsLength) {
    for (int i = 0; i < BitsLength; i++) {
        if (ReadBuffer[i] == '0') {
            if (Root->Left == NULL) {
                Root->Left = MakeNode('\0');
                Root->Left->Left = NULL;
                Root->Left->Right = NULL;
            }
            Root = Root->Left;
        }
        else if (ReadBuffer[i] == '1') {
            if (Root->Right == NULL) {
                Root->Right = MakeNode('\0');
                Root->Right->Left = NULL;
                Root->Right->Right = NULL;
            }
            Root = Root->Right;
        }
    }
    Root->Symbol = ReadSymbol;
}

// 'argc' and 'argv' contain the number of arguments and the values of arguments respectively.
int main(int argc, char **argv) {
    char TempCharacter;
    int CharNum = 0, Counter = 0;

    // Check if the number of arguments is 2. 
    //The another argument is a command and included in 'argc', so we need to check if a value of 'argc' is 3.
    if (argc != 3) {
        printf("The Number of arguments is larger or smaller. You need 2 arguments.");
        return 0;
    }

    if (strcmp(argv[1], "-c") == 0) {

        // 'argv[2]' contains a file name that we need to read.
        char *FileName = (char*)malloc((int)(strlen(argv[2]) + 1));
        snprintf(FileName, (int)(strlen(argv[2]) + 1), "%s", argv[2]);

        FILE *FileReadPointer = fopen(FileName, "rb");
        // Check if an error occurs while reading the file.
        if (FileReadPointer == NULL) {
            printf("File Read Error!\n");
            free(FileName);
            return -1;
        }

        while (fread(&TempCharacter, sizeof(char), 1, FileReadPointer) != 0) {
            MakePriorityQueue(TempCharacter, &CharNum);
        }


        /*-------------------------------------------------------
        테스트 용도 함수 
        CountQueue();
        printf("CharNum: %d\n", CharNum);
        -------------------------------------------------------*/


        fclose(FileReadPointer);

        TreeRoot = MakeTree(CharNum);

        char* TempBuffer = (char*)malloc(sizeof(char) + 1);
        snprintf(TempBuffer, sizeof(char) + 1, "\0");
        GenerateCode(TreeRoot, TempBuffer);


        /*-------------------------------------------------------
        테스트 용도 함수
        InorderTest(TreeRoot);
        -------------------------------------------------------*/


        FILE *InputFilePointer = fopen(FileName, "rb");
        if (InputFilePointer == NULL) {
            printf("File Read Error!\n");
            free(FileName);
            FreeQueue();
            return -1;
        }

        // Make a name of an output file.
        char* ResultFileName = (char*)malloc((int)strlen(FileName) + sizeof(char) * 3 + 1);
        snprintf(ResultFileName, (int)strlen(FileName) + sizeof(char) * 3 + 1, "%s.zz", FileName);


        // Produce the output file.
        FILE *OutputFilePointer = fopen(ResultFileName, "wb");
        // Check if an error occurs while writing the file.
        if (OutputFilePointer == NULL) {
            printf("File Write Error!\n");
            fclose(InputFilePointer);
            free(FileName);
            free(ResultFileName);
            FreeQueue();
            return 0;
        }

        fwrite(&CharNum, sizeof(int), 1, OutputFilePointer); // 전체 심볼의 개수

        for (int i = 1; i <= CharNum; i++) {
            Counter = 0;
            GenerateTreeData(OutputFilePointer, TreeRoot, &Counter, i);
        }

        LocationTotalBitNum = ftell(OutputFilePointer); // 전체 비트의 개수를 저장할 위치

        fwrite(&TotalBitNum, sizeof(int), 1, OutputFilePointer); // 전체 비트의 개수

        while (1) {
            int ReadFlag = fread(&TempCharacter, sizeof(char), 1, InputFilePointer);
            if (ReadFlag != 0) {
                TempRoot = TreeRoot;
                InorderTraverse(TempRoot, TempCharacter);
                MakeCompression(OutputFilePointer);
            }
            else {
                WriteBit(OutputFilePointer, 0, 1);
                break;
            }
        }

        if (fseek(OutputFilePointer, LocationTotalBitNum, SEEK_SET) == 0) {
            fwrite(&TotalBitNum, sizeof(int), 1, OutputFilePointer);

            /*-------------------------------------------------------
            테스트 용도
            //printf("Total Number of Bits: %d\nLocation of Total Number of Bits: %d\n", TotalBitNum, LocationTotalBitNum);
            -------------------------------------------------------*/

        }
        else {
            printf("File Write Error!\n");
        }

        fclose(InputFilePointer);
        fclose(OutputFilePointer);
        FreeTree(TreeRoot);
        free(FileName);
        free(ResultFileName);
    }
    else if (strcmp(argv[1], "-d") == 0) {

        // 'argv[2]' contains a file name that we need to read.
        char *FileName = (char*)malloc((int)(strlen(argv[2]) + 1));
        snprintf(FileName, (int)(strlen(argv[2]) + 1), "%s", argv[2]);

        FILE *FileReadPointer = fopen(FileName, "rb");
        // Check if an error occurs while reading the file.
        if (FileReadPointer == NULL) {
            printf("File Read Error!\n");
            free(FileName);
            return -1;
        }

        TreeRoot = MakeNode('\0');

        fread(&CharNum, sizeof(int), 1, FileReadPointer);


        /*-------------------------------------------------------
        테스트 용도
        //printf("Character Number: %d\n", CharNum);
        -------------------------------------------------------*/


        for (int i = 0; i < CharNum; i++) {
            char ReadBuffer[100];
            char SymbolData[2];
            TempRoot = TreeRoot;
            fread(SymbolData, 2 * sizeof(char), 1, FileReadPointer);
            fread(ReadBuffer, 1, (int)SymbolData[1], FileReadPointer);
            ReadBuffer[(int)SymbolData[1]] = '\0';

            /*-------------------------------------------------------
            테스트 용도
            //printf("Symbol: %c | Code Length: %d | Code: %s\n", SymbolData[0], SymbolData[1], ReadBuffer);
            -------------------------------------------------------*/
            RecoverTree(TempRoot, ReadBuffer, SymbolData[0], (int)SymbolData[1]);
        }


        /*-------------------------------------------------------
        테스트 용도
        InorderTest(TreeRoot);
        printf("Location of Total Number of Bits: %d\n", ftell(FileReadPointer));
        -------------------------------------------------------*/


        fread(&TotalBitNum, sizeof(int), 1, FileReadPointer); // 전체 비트 개수 읽기


        // Make a name of an output file.
        char* ResultFileName = (char*)malloc((int)strlen(FileName) + sizeof(char) * 3 + 1);
        snprintf(ResultFileName, (int)strlen(FileName) + sizeof(char) * 3 + 1, "%s.yy", FileName);

        // Produce the output file.
        FILE *OutputFilePointer = fopen(ResultFileName, "wb");
        // Check if an error occurs while writing the file.
        if (OutputFilePointer == NULL) {
            printf("File Write Error!\n");
            free(FileName);
            free(ResultFileName);
            FreeQueue();
            return 0;
        }

        TempRoot = TreeRoot;
        while (fread(&TempCharacter, sizeof(char), 1, FileReadPointer) != 0) {
            int FinishFlag = 0;
            for (int i = 0; i < 8; i++) {
                if ((char)(TempCharacter & 0x80) == 0) { // LSB 비트 AND MASK
                    TempRoot = TempRoot->Left;
                }
                else {
                    TempRoot = TempRoot->Right;
                }
                TempCharacter = TempCharacter << 1; // 왼쪽으로 비트 이동
                TotalBitNum--;

                if (TempRoot->Left == NULL && TempRoot->Right == NULL) { // Huffman Tree에서 깊이 끝까지 도달했으면 leaf 노드이므로
                    fprintf(OutputFilePointer, "%c", TempRoot->Symbol);
                    TempRoot = TreeRoot;
                }
                if (TotalBitNum == 0) {
                    FinishFlag = 1;
                    break;
                }
            }
            if (FinishFlag == 1) {
                break;
            }
        }

        // 동적할당하거나 파일 연 것 모두 초기화
        fclose(FileReadPointer);
        fclose(OutputFilePointer);
        FreeTree(TreeRoot);
        free(FileName);
        free(ResultFileName);
    }
    return 0;
}