#include "20171665.h"

int ExecutionAddress = 0;

int LoadObjectCode(int ModifiedLengthValue, int ModifiedOddLengthFlag, int CSADDR, int AddressValue, char OperatorCharacter, EXTSYMBOL *TempSymbol, int AddressBlankFlag) {
    char *ModifiedMemory = NULL, TempHexDigit, TempFirstByte[3] = { '\0' };
    int ModifiedValue = 0, TempModifiedValue = 0;
    ModifiedMemory = (char*)malloc(sizeof(char) * ((ModifiedLengthValue + 1) / 2) * 2 + 1);
    memset(ModifiedMemory, '\0', sizeof(char) * ((ModifiedLengthValue + 1) / 2) * 2 + 1);

    for (int i = 0; i < (ModifiedLengthValue + 1) / 2; i++) {
		// displacement에서 수정해야 할 코드 길이가 홀수이고 첫 번째 halfbyte이면
        if (ModifiedOddLengthFlag && !i) {
            sprintf(TempFirstByte, "%02X", DumpMemory[CSADDR + AddressValue + i] & 0XF0);
            //printf("TempFirstByte: %s\n", TempFirstByte);
            TempHexDigit = TempFirstByte[0];
            //printf("TempHexDigit: %c\n", TempHexDigit);
            sprintf(ModifiedMemory, "0%01X", DumpMemory[CSADDR + AddressValue + i] & 0X0F);
        }
        else {
            sprintf(ModifiedMemory, "%s%02X", ModifiedMemory, DumpMemory[CSADDR + AddressValue + i] & 0XFF);
        }
    }

    ModifiedMemory[(ModifiedLengthValue + 1) / 2 * 2] = '\0';

    if (ModifiedOddLengthFlag) {
        TempModifiedValue = HexCharToDec(ModifiedMemory[1]);
		// displacement가 음수일 때
        if ((TempModifiedValue & 0X8) == 0X8) {
            sprintf(ModifiedMemory, "0%01X%s", TempModifiedValue & 0X7, ModifiedMemory + 2);
            ModifiedValue = (-1)*(0X8 * (int)pow(16, ModifiedLengthValue - 1)) + HexStringToDec(ModifiedMemory, ForHexAddress);
        }
        else {
            ModifiedValue = HexStringToDec(ModifiedMemory, ForHexAddress);
        }
    }
    else {
        TempModifiedValue = HexCharToDec(ModifiedMemory[0]);
		// value가 음수일 때
        if ((TempModifiedValue & 0X8) == 0X8) {
            sprintf(ModifiedMemory, "%01X%s", TempModifiedValue & 0X7, ModifiedMemory + 1);
            ModifiedValue = (-1)*(0X8 * (int)pow(16, ModifiedLengthValue - 1)) + HexStringToDec(ModifiedMemory, ForHexAddress);
        }
        else {
            ModifiedValue = HexStringToDec(ModifiedMemory, ForHexAddress);
        }
    }

    //printf("1: %s / %d / %04X\n", ModifiedMemory, ModifiedValue, TempSymbol->SymbolAddress & 0XFFFF);

    if (!AddressBlankFlag) {
        if (OperatorCharacter == '+') {
            ModifiedValue += (TempSymbol->SymbolAddress & 0XFFFFF);
        }
        else if (OperatorCharacter == '-') {
            ModifiedValue -= (TempSymbol->SymbolAddress & 0XFFFFF);
        }
    }
    else {
        if (OperatorCharacter == '+') {
            ModifiedValue += (CSADDR & 0XFFFFF);
        }
        else if (OperatorCharacter == '-') {
            ModifiedValue -= (CSADDR & 0XFFFFF);
        }
        else {
            ModifiedValue += (CSADDR & 0XFFFFF);
        }
    }

    //printf("2: %s / %d / %04X\n", ModifiedMemory, ModifiedValue, TempSymbol->SymbolAddress & 0XFFFF);

    memset(ModifiedMemory, '\0', sizeof(char) * ((ModifiedLengthValue + 1) / 2) * 2 + 1);
    sprintf(ModifiedMemory, "%06X", ModifiedValue & 0XFFFFFF);
    //printf("[%s]\n", ModifiedMemory);

    for (int i = 0; i < (ModifiedLengthValue + 1) / 2; i++) {
        char MemoryString[3] = { '\0' };
        if (ModifiedOddLengthFlag && !i) {
            sprintf(MemoryString, "%c%c", TempHexDigit, ModifiedMemory[1]);
        }
        else {
            strncpy(MemoryString, ModifiedMemory + i * 2, 2);
            //printf("MemoryString: %s\n", MemoryString);
        }
        MemoryString[2] = '\0';
        DumpMemory[CSADDR + AddressValue + i] = HexStringToDec(MemoryString, ForHexAddress);
    }

    //printf("[%s]\n", ModifiedMemory);

    if (ModifiedMemory != NULL) {
        free(ModifiedMemory);
        ModifiedMemory = NULL;
    }

    return 0;
}

int LoadPassTwo(char *FileName1, char *FileName2, char *FileName3) {
    FILE *FilePointer1 = NULL, *FilePointer2 = NULL, *FilePointer3 = NULL, *TempFilePointer = NULL;
    int LoadedFileNumber = 0, CSADDR = 0, CSLTH = 0, LoopCounter = 0;
    int ErrorFlag = 0, EndFlag = 0;

    if (FileName1 != NULL) {
        if ((FilePointer1 = fopen(FileName1, "r")) != NULL) {
            LoadedFileNumber += 1;
        }
        else {
            ErrorFlag = 1;
        }
        if (FileName2 != NULL) {
            if ((FilePointer2 = fopen(FileName2, "r")) != NULL) {
                LoadedFileNumber += 1;
            }
            else {
                ErrorFlag = 1;
            }
            if (FileName3 != NULL) {
                if ((FilePointer3 = fopen(FileName3, "r")) != NULL) {
                    LoadedFileNumber += 1;
                }
                else {
                    ErrorFlag = 1;
                }
            }
        }
    }

    if (ErrorFlag == 1) {
        return 1;
    }

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

        char ReadString[100], *ReadLine = NULL, AddressString[7], LengthString[3], NameString[7], **ReferenceTable = NULL;
        int AddressValue = 0, LengthValue = 0;
        EXTSYMBOL *TempSymbol = NULL;

        memset(ReadString, '\0', sizeof(ReadString));
        memset(NameString, '\0', sizeof(NameString));
        memset(LengthString, '\0', sizeof(LengthString));
        memset(AddressString, '\0', sizeof(AddressString));

        ReadLine = fgets(ReadString, 20, TempFilePointer);

        //printf("ReadString: %s / ReadLine: %s\n", ReadString, ReadLine);

        if (ReadLine[0] == 'H') {
            CSLTH = HexStringToDec(ReadLine + 13, ForHexAddress);
            strncpy(AddressString, ReadLine + 7, 6);
            AddressString[6] = '\0';
            TrimRightBlank(AddressString);

            strncpy(NameString, ReadLine + 1, 6);
            NameString[6] = '\0';
            TrimRightBlank(NameString);
            //printf("AddressString: %s\n", AddressString);
            CSADDR += HexStringToDec(AddressString, ForHexAddress);
        }
        else {
			printf("The first line should be a header line.\n");
            ErrorFlag = 1;
            break;
        }

        while (!feof(TempFilePointer)) {
            memset(ReadString, '\0', sizeof(ReadString));
            memset(LengthString, '\0', sizeof(LengthString));
            memset(AddressString, '\0', sizeof(AddressString));

            ReadLine = fgets(ReadString, 74, TempFilePointer);
            //printf("ReadString: %s / ReadLine: %s\n", ReadString, ReadLine);
            if (ReadLine[0] == 'E') {
                if (ReadLine[1] != '\0') {
                    strncpy(AddressString, ReadLine + 1, 6);
                    AddressString[6] = '\0';
                    TrimRightBlank(AddressString);
                    if (AddressString[0] != '\0') {
                        AddressValue = HexStringToDec(AddressString, ForHexAddress);
                        ExecutionAddress = CSADDR + AddressValue;
                    }
                }
                EndFlag = 1;
                break;
            }
            else if (ReadLine[0] == 'R') {
                if ((ReadLine[1] >= '0' && ReadLine[1] <= '9') && (ReadLine[2] >= '0' && ReadLine[2] <= '9')) {
                    ReferenceTable = (char**)malloc(sizeof(char*) * 50);
                    for (int i = 0; i < 50; i++) {
                        ReferenceTable[i] = (char*)malloc(sizeof(char) * 7);
                        memset(ReferenceTable[i], '\0', 7);
                    }
                    
                    sprintf(ReferenceTable[1], "%s", NameString);

                    for (int i = 0; i < 12; i++) {
                        char ReferenceNumber[3] = { '\0' };
                        int ReferenceIndex = 0;

                        strncpy(ReferenceNumber, ReadLine + i * 8 + 1, 2);
                        ReferenceNumber[2] = '\0';
                        TrimRightBlank(ReferenceNumber);
                        ReferenceIndex = DecStringToDec(ReferenceNumber, Word);
                        //printf("Reference Index: %d\n", ReferenceIndex);
                        strncpy(ReferenceTable[ReferenceIndex], ReadLine + i * 8 + 3, 6);
                        ReferenceTable[ReferenceIndex][6] = '\0';
                        TrimRightBlank(ReferenceTable[ReferenceIndex]);
                    }

                    /*
                    for (int i = 0; i < 50; i++) {
                        printf("%d: %s", i, ReferenceTable[i]);
                    }
                    printf("\n");
                    */

                }
            }
            else if (ReadLine[0] == 'T') {
                strncpy(AddressString, ReadLine + 1, 6);
                AddressString[6] = '\0';
                TrimRightBlank(AddressString);
                AddressValue = HexStringToDec(AddressString, ForHexAddress);
                strncpy(LengthString, ReadLine + 7, 2);
                LengthString[2] = '\0';
                LengthValue = HexStringToDec(LengthString, ForHexAddress);

                for (int i = 0; i < LengthValue; i++) {
                    char MemoryString[3] = { '\0' };

                    strncpy(MemoryString, ReadLine + 9 + (i * 2), 2);
                    MemoryString[2] = '\0';
                    DumpMemory[CSADDR + AddressValue + i] = HexStringToDec(MemoryString, ForHexAddress);
                }
            }
            else if (ReadLine[0] == 'M') {
                char OperatorCharacter, ModifiedLength[3] = { '\0' };
                int ModifiedLengthValue = 0, ModifiedOddLengthFlag = 0, AddressBlankFlag = 0;

                strncpy(AddressString, ReadLine + 1, 6);
                AddressString[6] = '\0';
                TrimRightBlank(AddressString);
                AddressValue = HexStringToDec(AddressString, ForHexAddress);

                OperatorCharacter = ReadLine[9];

                strncpy(ModifiedLength, ReadLine + 7, 2);
                ModifiedLength[2] = '\0';
                TrimRightBlank(ModifiedLength);
                ModifiedLengthValue = DecStringToDec(ModifiedLength, Word);

                if (ModifiedLengthValue % 2 != 0) {
                    ModifiedOddLengthFlag = 1;
                }

                if (ReferenceTable == NULL) {
                    char SymbolString[7] = { '\0' };

                    strncpy(SymbolString, ReadLine + 10, 6);
                    SymbolString[6] = '\0';
                    TrimRightBlank(SymbolString);

                    if (SymbolString[0] == '\0') {
                        AddressBlankFlag = 1;
                        if (LoadObjectCode(ModifiedLengthValue, ModifiedOddLengthFlag, CSADDR, AddressValue, OperatorCharacter, TempSymbol, AddressBlankFlag)) {
                            ErrorFlag = 1;
                            break;
                        }
                    }
                    else {
                        if (!SearchESTAB(SymbolString, &TempSymbol)) {
                            if (LoadObjectCode(ModifiedLengthValue, ModifiedOddLengthFlag, CSADDR, AddressValue, OperatorCharacter, TempSymbol, AddressBlankFlag)) {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        else {
							printf("The symbol does not exist in the external symbol table.\n");
                            ErrorFlag = 1;
                            break;
                        }
                    }     
                }
                else {
                    char ReferenceIndexString[3] = { '\0' };
                    int ReferenceIndex = 0, AddressBlankFlag = 0;

                    //printf("Modified Half Byte Length: %d / Operator: %c\n", ModifiedLengthValue, OperatorCharacter);

                    strncpy(ReferenceIndexString, ReadLine + 10, 2);
                    ReferenceIndexString[2] = '\0';
                    TrimRightBlank(ReferenceIndexString);

                    if (ReferenceIndexString[0] == '\0') {
                        AddressBlankFlag = 1;
                        if (LoadObjectCode(ModifiedLengthValue, ModifiedOddLengthFlag, CSADDR, AddressValue, OperatorCharacter, TempSymbol, AddressBlankFlag)) {
                            ErrorFlag = 1;
                            break;
                        }
                    }
                    else {
                        ReferenceIndex = DecStringToDec(ReferenceIndexString, Word);
                        if (!SearchESTAB(ReferenceTable[ReferenceIndex], &TempSymbol)) {
                            if (LoadObjectCode(ModifiedLengthValue, ModifiedOddLengthFlag, CSADDR, AddressValue, OperatorCharacter, TempSymbol, AddressBlankFlag)) {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        else {
							printf("The symbol does not exist in the external symbol table.\n");
                            ErrorFlag = 1;
                            break;
                        }
                    }

                    //("RI: %d / [%s]\n", ReferenceIndex, ReferenceTable[ReferenceIndex]);
                    
                }
            }

        }

        if (ReferenceTable != NULL) {
            /*
            printf("Loop Counter: %d\n", LoopCounter);
            for (int i = 0; i < 50; i++) {
                printf("%d: %s", i, ReferenceTable[i]);
            }
            printf("\n");
            */

            for (int i = 0; i < 50; i++) {
                free(ReferenceTable[i]);
            }

            free(ReferenceTable);
        }

        if (EndFlag == 0) {
            ErrorFlag = 1;
            break;
        }

        CSADDR += CSLTH;
        LoopCounter++;
    }

    if (ErrorFlag) {
        printf("An error occurs.\n");
        return 1;
    }

    return 0;
}