#include "20171665.h"

EXTSYMBOL ESTAB[20];
int ProgramTotalLength = 0;

void TrimRightBlank(char *String) {
    char TempString[100], *StringEnd = NULL;

    strcpy(TempString, String);
    StringEnd = TempString + strlen(TempString) - 1;

    while (StringEnd != TempString && (*StringEnd == ' ' || *StringEnd == '\0' || *StringEnd == '\n')) {
        StringEnd--;
    }

    *(StringEnd + 1) = '\0';
    strcpy(String, TempString);

    return;
}

/*------------------------------------------------------------------------------------*/
/*�Լ� : InitializeESTAB()*/
/*���� : External Symbol Table�� ��� �� ���·� �ʱ�ȭ�Ѵ�.*/
/*���ϰ� : ���������� ����Ǹ� 0�� ��ȯ�Ѵ�.*/
/*------------------------------------------------------------------------------------*/
int InitializeESTAB() {
    int Index;
    char *LabelNamePointer = NULL;

    for (Index = 0; Index < 20; Index++) {
        if ((LabelNamePointer = ESTAB[Index].SymbolName) != NULL) {
            free(LabelNamePointer);
        }
        LabelNamePointer = NULL;
        ESTAB[Index].SymbolAddress = 0;
        ESTAB[Index].ControlSectionLength = 0;
        ESTAB[Index].OrderNumber = 0;
        ESTAB[Index].ControlSectionOrNot = 0;
        ESTAB[Index].Next = NULL;
    }

    return 0;
}

/*------------------------------------------------------------------------------------*/
/*�Լ� : InsertESTAB(char *Name, int LocationCounter, int ControlSectionOrNot, int OrderNumber, int ControlSectionLength)*/
/*���� : HashFunction�� ����Ͽ� External Symbol�� �����Ǵ� Hash ���� ���Ͽ� �ش� ��° linked list �ڿ� �����ϴ� ����� �����Ѵ�.*/
/*���ϰ� : ���������� ����Ǹ� 0�� ��ȯ�Ѵ�.*/
/*------------------------------------------------------------------------------------*/
int InsertESTAB(char *Name, int LocationCounter, int ControlSectionOrNot, int OrderNumber, int ControlSectionLength) {
    int Buffer;

    // ESTAB�� �ش� hash���� ���� index�� Name�� ��� ������
    if (ESTAB[(Buffer = HashFunction(Name))].SymbolName == NULL) {

        ESTAB[Buffer].SymbolName = (char*)malloc(sizeof(char) * (int)strlen(Name) + 1);
        strcpy(ESTAB[Buffer].SymbolName, Name);
        ESTAB[Buffer].SymbolName[strlen(Name)] = '\0';

        if (ControlSectionOrNot == IsControlSection) {
            ESTAB[Buffer].ControlSectionOrNot = IsControlSection;
        }
        else if (ControlSectionOrNot == IsSymbol) {
            ESTAB[Buffer].ControlSectionOrNot = IsSymbol;
        }

        ESTAB[Buffer].SymbolAddress = LocationCounter;
        ESTAB[Buffer].OrderNumber = OrderNumber;
        ESTAB[Buffer].ControlSectionLength = ControlSectionLength;
        ESTAB[Buffer].Next = NULL;
    }
    else {
        EXTSYMBOL *NewSymbol = NULL, *TempSymbol = &(ESTAB[Buffer]);

        NewSymbol = (EXTSYMBOL*)malloc(sizeof(EXTSYMBOL));
        NewSymbol->SymbolName = (char*)malloc(sizeof(char) * (int)strlen(Name) + 1);
        strcpy(NewSymbol->SymbolName, Name);
		NewSymbol->SymbolName[strlen(Name)] = '\0';

        if (ControlSectionOrNot == IsControlSection) {
            NewSymbol->ControlSectionOrNot = IsControlSection;
        }
        else if (ControlSectionOrNot == IsSymbol) {
            NewSymbol->ControlSectionOrNot = IsSymbol;
        }
        NewSymbol->SymbolAddress = LocationCounter;
        NewSymbol->OrderNumber = OrderNumber;
        NewSymbol->ControlSectionLength = ControlSectionLength;
        NewSymbol->Next = NULL;

        while (TempSymbol->Next != NULL) {
            TempSymbol = TempSymbol->Next;
        }
        TempSymbol->Next = NewSymbol;
    }

    return 0;
}

/*------------------------------------------------------------------------------------*/
/*�Լ� : DeleteESTAB()*/
/*���� : External Symbol Table�� ����ϱ� ���� ������ ����Ǿ� �ִ� table�� ������ �����.*/
/*���ϰ� : ����*/
/*------------------------------------------------------------------------------------*/
void DeleteESTAB() {
    int Index;
    EXTSYMBOL *TempSymbol = NULL, *DeleteSymbol = NULL;

    for (Index = 0; Index < 20; Index++) {
        TempSymbol = &(ESTAB[Index]);
        if (TempSymbol->SymbolName != NULL) {
            free(TempSymbol->SymbolName);
            TempSymbol->SymbolName = NULL;
            if ((TempSymbol = TempSymbol->Next) != NULL) {
                while (TempSymbol != NULL) {
                    DeleteSymbol = TempSymbol;
                    TempSymbol = TempSymbol->Next;
                    if (DeleteSymbol->SymbolName != NULL) {
                        free(DeleteSymbol->SymbolName);
                    }
                    free(DeleteSymbol);
                }
            }
        }
    }

    return;
}

/*------------------------------------------------------------------------------------*/
/*�Լ� : SearchESTAB(char *Name, EXTSYMBOL **TempPointer)*/
/*���� : ã���� �ϴ� External Symbol Name�� Symbol Table�� �����ϴ��� HashFunction �Լ��� ����Ͽ� hash ���� ���� �� Symbol �迭�� �ش� ���ҷ� �����Ͽ� Linked list�� Ž���ϴ� �뵵�� ���δ�.*/
/*���ϰ� : ���������� ����Ǹ� 0�� ��ȯ�Ѵ�.*/
/*------------------------------------------------------------------------------------*/
int SearchESTAB(char *Name, EXTSYMBOL **TempPointer) {
    int HashCode;
    EXTSYMBOL* TempSymbol = NULL;

    if (!((HashCode = HashFunction(Name)) >= 0 && HashCode < 20)) {
        printf("An error occurs while reading symbol table.\n");
        return -1;
    }

    if ((TempSymbol = &(ESTAB[HashCode])) == NULL) {
        printf("An error occurs while reading symbol table.\n");
        return -1;
    }

    if (TempSymbol->SymbolName != NULL) {
        while ((strcmp(TempSymbol->SymbolName, Name) != 0)) {
            // Label�� Symbol Table���� ã�� ���ϸ�
            if (TempSymbol->Next == NULL) {
                return 1;
            }
            TempSymbol = TempSymbol->Next;
        }
    }
    else {
        return 1;
    }

    (*TempPointer) = TempSymbol;

    // Label�� Symbol Table���� ã����
    return 0;
}

/*------------------------------------------------------------------------------------*/
/*�Լ� : PrintESTAB()*/
/*���� : External Symbol Table�� ����� symbol���� �ּ��� ������������ â�� ����ϴ� ������ �Ѵ�.*/
/*���ϰ� : ���������� ����Ǹ� 0�� ��ȯ�Ѵ�.*/
/*------------------------------------------------------------------------------------*/
int PrintESTAB() {
    int Index, SymbolNumber = 0, TempValue = 0, TotalLength = 0;
    EXTSYMBOL *TempSymbol = NULL, *SortedSymbolList = NULL;
    char *TempSymbolName = NULL;

    for (Index = 0; Index < 20; Index++) {
        TempSymbol = &(ESTAB[Index]);
        if (TempSymbol->SymbolName != NULL) {
            while (TempSymbol->Next != NULL) {
                SymbolNumber++;
                TempSymbol = TempSymbol->Next;
            }
            SymbolNumber++;
        }
    }

    if (SymbolNumber) {
        SortedSymbolList = (EXTSYMBOL*)malloc(sizeof(EXTSYMBOL) * SymbolNumber);
        for (int i = 0; i < SymbolNumber; i++) {
            SortedSymbolList[i].SymbolName = NULL;
            SortedSymbolList[i].SymbolAddress = 0;
            SortedSymbolList[i].Next = NULL;
        }

        for (Index = 0, SymbolNumber = 0; Index < 20; Index++) {
            TempSymbol = &(ESTAB[Index]);
            if (TempSymbol->SymbolName != NULL) {
                while (TempSymbol->Next != NULL) {
                    SortedSymbolList[SymbolNumber].SymbolName = (char*)malloc(sizeof(char) * (int)strlen(TempSymbol->SymbolName) + 1);
                    strcpy(SortedSymbolList[SymbolNumber].SymbolName, TempSymbol->SymbolName);
                    SortedSymbolList[SymbolNumber].ControlSectionLength = TempSymbol->ControlSectionLength;
                    SortedSymbolList[SymbolNumber].ControlSectionOrNot = TempSymbol->ControlSectionOrNot;
                    SortedSymbolList[SymbolNumber].OrderNumber = TempSymbol->OrderNumber;
                    SortedSymbolList[SymbolNumber++].SymbolAddress = TempSymbol->SymbolAddress;
                    TempSymbol = TempSymbol->Next;
                }
                SortedSymbolList[SymbolNumber].SymbolName = (char*)malloc(sizeof(char) * (int)strlen(TempSymbol->SymbolName) + 1);
                strcpy(SortedSymbolList[SymbolNumber].SymbolName, TempSymbol->SymbolName);
                SortedSymbolList[SymbolNumber].ControlSectionLength = TempSymbol->ControlSectionLength;
                SortedSymbolList[SymbolNumber].ControlSectionOrNot = TempSymbol->ControlSectionOrNot;
                SortedSymbolList[SymbolNumber].OrderNumber = TempSymbol->OrderNumber;
                SortedSymbolList[SymbolNumber++].SymbolAddress = TempSymbol->SymbolAddress;
            }
        }

        if (SymbolNumber > 1) {
            for (int i = 0; i < SymbolNumber; i++) {
                for (int j = i + 1; j < SymbolNumber; j++) {
                    if (SortedSymbolList[i].OrderNumber > SortedSymbolList[j].OrderNumber) {
                        TempSymbolName = SortedSymbolList[i].SymbolName;
                        SortedSymbolList[i].SymbolName = SortedSymbolList[j].SymbolName;
                        SortedSymbolList[j].SymbolName = TempSymbolName;

                        TempValue = SortedSymbolList[i].SymbolAddress;
                        SortedSymbolList[i].SymbolAddress = SortedSymbolList[j].SymbolAddress;
                        SortedSymbolList[j].SymbolAddress = TempValue;

                        TempValue = SortedSymbolList[i].ControlSectionLength;
                        SortedSymbolList[i].ControlSectionLength = SortedSymbolList[j].ControlSectionLength;
                        SortedSymbolList[j].ControlSectionLength = TempValue;

                        TempValue = SortedSymbolList[i].ControlSectionOrNot;
                        SortedSymbolList[i].ControlSectionOrNot = SortedSymbolList[j].ControlSectionOrNot;
                        SortedSymbolList[j].ControlSectionOrNot = TempValue;

                        TempValue = SortedSymbolList[i].OrderNumber;
                        SortedSymbolList[i].OrderNumber = SortedSymbolList[j].OrderNumber;
                        SortedSymbolList[j].OrderNumber = TempValue;
                    }
                }
            }
        }

        printf("control\t\tsymbol\t\taddress\t\tlength\n");
        printf("section\t\tname\n");
        printf("-------------------------------------------------------\n");

        for (int i = 0; i < SymbolNumber; i++) {
            if (SortedSymbolList[i].ControlSectionOrNot == IsControlSection) {
                printf("%s\t\t\t\t%05X\t\t%05X\n", SortedSymbolList[i].SymbolName, SortedSymbolList[i].SymbolAddress & 0XFFFFF, SortedSymbolList[i].ControlSectionLength & 0XFFFFF);
                TotalLength += SortedSymbolList[i].ControlSectionLength;
            }
            else if (SortedSymbolList[i].ControlSectionOrNot == IsSymbol) {
                printf("\t\t%s\t\t%05X\n", SortedSymbolList[i].SymbolName, SortedSymbolList[i].SymbolAddress & 0XFFFFF);
            }
        }

        ProgramTotalLength = TotalLength;

        printf("-------------------------------------------------------\n");
        printf("total length:\t%05X\n", TotalLength & 0XFFFFF);

        for (int i = 0; i < SymbolNumber; i++) {
            free(SortedSymbolList[i].SymbolName);
        }
        free(SortedSymbolList);
    }

    return 0;
}

/*------------------------------------------------------------------------------------*/
/*�Լ� : LoadPassOne(char *FileName1, char *FileName2, char *FileName3)*/
/*���� : Object Code�� �޸𸮿� load�ϱ� ���ؼ��� ���� ���ǵ� ��ȣ�� �ܺ� ���� ��ȣ�� ���� �������� �ּҸ� �̸� ���Ͽ� symbol�� �����Ǵ� �ּҸ� �����ϰ��� �� ���� �бⰡ �ʿ��ѵ�, �� �� ù ��° �б��� Pass 1�� �����ϴ� �Լ��̴�. */
/*���ϰ� : ���������� ����Ǹ� 0�� ��ȯ�Ѵ�.*/
/*------------------------------------------------------------------------------------*/

int LoadPassOne(char *FileName1, char *FileName2, char *FileName3) {
    FILE *FilePointer1 = NULL, *FilePointer2 = NULL, *FilePointer3 = NULL, *TempFilePointer = NULL;
    int LoadedFileNumber = 0, CSADDR = 0, CSLTH = 0, LoopCounter = 0, SymbolCounter = 0;
    int ErrorFlag = 0, EndFlag = 0;

    if (FileName1 != NULL) {
        if ((FilePointer1 = fopen(FileName1, "r")) != NULL) {
            LoadedFileNumber += 1;
        }
        else {
            printf("Please write the name of the first valid file that can be read.\n");
            ErrorFlag = 1;
        }
        if (FileName2 != NULL) {
            if ((FilePointer2 = fopen(FileName2, "r")) != NULL) {
                LoadedFileNumber += 1;
            }
            else {
                printf("Please write the name of the second valid file that can be read.\n");
                ErrorFlag = 1;
            }
            if (FileName3 != NULL) {
                if ((FilePointer3 = fopen(FileName3, "r")) != NULL) {
                    LoadedFileNumber += 1;
                }
                else {
                    printf("Please write the name of the third valid file that can be read.\n");
                    ErrorFlag = 1;
                }
            }
        }
    }

    if (ErrorFlag == 1) {
        printf("An error occurs. Please check the error.\n");
        return 1;
    }

    DeleteESTAB();
    InitializeESTAB();
    CSADDR = ProgramStartAddress;

    while ((LoopCounter < LoadedFileNumber) && !ErrorFlag) {
        switch (LoopCounter) {
        case 0:
            TempFilePointer = FilePointer1; break;
        case 1:
            TempFilePointer = FilePointer2; break;
        case 2:
            TempFilePointer = FilePointer3; break;
        default:
            printf("The number of loaded file is over the limit.\n");
            ErrorFlag = 1;
            break;
        }

        char ReadString[100], *ReadLine = NULL, NameString[7], AddressString[7];
        EXTSYMBOL *TempSymbol = NULL;

        memset(ReadString, '\0', sizeof(ReadString));
        memset(NameString, '\0', sizeof(NameString));
        memset(AddressString, '\0', sizeof(AddressString));

        ReadLine = fgets(ReadString, 20, TempFilePointer);

        //printf("ReadString: %s / ReadLine: %s\n", ReadString, ReadLine);

        if (ReadLine[0] == 'H') {
            CSLTH = HexStringToDec(ReadLine + 13, ForHexAddress);
            strncpy(NameString, ReadLine + 1, 6);
            NameString[6] = '\0';
            TrimRightBlank(NameString);
            strncpy(AddressString, ReadLine + 7, 6);
            AddressString[6] = '\0';
            TrimRightBlank(AddressString);
            if (!SearchESTAB(NameString, &TempSymbol)) {
                ErrorFlag = 1;
                break;
            }
            else {
                CSADDR += HexStringToDec(AddressString, ForHexAddress);
                if (CSADDR + CSLTH > 0XFFFFF){
                    ErrorFlag = 1;
                    break;
                }
                InsertESTAB(NameString, CSADDR, IsControlSection, (++SymbolCounter), CSLTH);
            }
        }
        else {
            ErrorFlag = 1;
            break;
        }

        while (!feof(TempFilePointer)) {
            memset(ReadString, '\0', sizeof(ReadString));

            ReadLine = fgets(ReadString, 74, TempFilePointer);
            //printf("ReadString: %s / ReadLine: %s\n", ReadString, ReadLine);
            if (ReadLine[0] == 'E') {
                EndFlag = 1;
                break;
            }
            else if (ReadLine[0] == 'D') {
                int TempIndex = 0, AddressValue;
                for (int i = 0; i < 6; i++) {
                    TempIndex = i * 12 + 1;
                    if (*(ReadLine + TempIndex) != '\0') {
                        memset(NameString, '\0', sizeof(NameString));
                        memset(AddressString, '\0', sizeof(AddressString));
                        strncpy(NameString, ReadLine + TempIndex, 6);
                        NameString[6] = '\0';
                        TrimRightBlank(NameString);
                        if (*(ReadLine + TempIndex + 6) != '\0') {
                            strncpy(AddressString, ReadLine + TempIndex + 6, 6);
                            AddressString[6] = '\0';
                            TrimRightBlank(AddressString);
                            //printf("NameString: %s", NameString);
                            //printf("AddressString: %s\n", AddressString);
                            AddressValue = HexStringToDec(AddressString, ForHexAddress);
                            if (!SearchESTAB(NameString, &TempSymbol)) {
                                ErrorFlag = 1;
                                break;
                            }
                            else {
                                AddressValue += CSADDR;
                                InsertESTAB(NameString, AddressValue, IsSymbol, (++SymbolCounter), 0);
                            }
                        }
                    }
                }
            }
        }

        if (EndFlag == 0) {
            ErrorFlag = 1;
            break;
        }

        CSADDR += CSLTH;
        LoopCounter++;
    }

    PrintESTAB();

    if (ErrorFlag) {
        printf("An error occurs. Please check the error.\n");
		ResetCommand();
        return 1;
    }

    return 0;
}