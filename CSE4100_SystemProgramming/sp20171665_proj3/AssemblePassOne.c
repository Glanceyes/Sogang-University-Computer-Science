#include "20171665.h"

/*------------------------------------------------------------------------------------*/
/*함수 : AssemblePassOne(char *AssembleFileName)*/
/*목적 : Pass 1*/
/*리턴값 : 0이면 성공, -1이면 실패*/
/*------------------------------------------------------------------------------------*/
int AssemblePassOne(char *AssembleFileName) {
    FILE *FilePointer = NULL, *ImmediateFilePointer = NULL;

    if ((FilePointer = fopen(AssembleFileName, "r")) == NULL) {
        printf("An error occurs while opening the file you want to assemble.\n");
        return -1;
    }

    char *FileName = NULL, *ImmediateFileName = NULL, *ListFileName = NULL, *ObjectFileName = NULL;
    int FileNameLength = 0, ErrorFlag = 0;

    if (strlen(AssembleFileName) <= 4) {
        printf("Please write the valid file format. It should be '*.asm'.\n");
        return -1;
    }

    FileName = (char*)malloc((FileNameLength = sizeof(char) * (int)strlen(AssembleFileName) + 1) - 4);
    ImmediateFileName = (char*)malloc(FileNameLength);

    strncpy(FileName, AssembleFileName, FileNameLength - 5);
    FileName[FileNameLength - 5] = '\0';

    snprintf(ImmediateFileName, FileNameLength, "%s.imm", FileName);
    ImmediateFileName[FileNameLength - 1] = '\0';

    if ((ImmediateFilePointer = fopen(ImmediateFileName, "w")) == NULL) {
        printf("An error occurs while writing the immediate file.\n");
        if (FileName != NULL) {
            free(FileName);
        }
        if (ImmediateFileName != NULL) {
            free(ImmediateFileName);
        }
        return -1;
    }

    ListFileName = (char*)malloc(FileNameLength);
    ObjectFileName = (char*)malloc(FileNameLength);

    snprintf(ListFileName, FileNameLength, "%s.lst", FileName);
    ListFileName[FileNameLength - 1] = '\0';

    snprintf(ObjectFileName, FileNameLength, "%s.obj", FileName);
    ObjectFileName[FileNameLength - 1] = '\0';

    int LabelCounter = 0, OpcodeMnemonicCounter = 0, OperandCounter = 0, AddLocationCounter = 0;
    int StartingAddress = 0, LocationCounter = 0, LineNumber = 0;
    int Flag = 0, NotAddressFlag = 0, StartFlag = 0, EndFlag = 0, CharacterFlag = 0, CommentFlag = 0, NowStartFlag = 0;
    char BufferChar = EOF, BeforeChar = EOF, *Label = NULL, *OpcodeMnemonic = NULL, *Operand = NULL;

    // 기존의 Symbol Table을 삭제하고 초기화
    DeleteSymbolTable();
    InitializeSymbolTable();

    while (!EndFlag) {

        Label = (char*)realloc(Label, sizeof(char) * (LabelCounter + 1));
        OpcodeMnemonic = (char*)realloc(OpcodeMnemonic, sizeof(char) * (OpcodeMnemonicCounter + 1));
        Operand = (char*)realloc(Operand, sizeof(char) * (OperandCounter + 1));

        LineNumber += 5;

        while ((BufferChar = fgetc(FilePointer)) != '\n') {
            // Line의 첫 character로 '.'를 읽으면 다음 line으로 넘어간다.
            if ((LabelCounter == 0) && (BufferChar == '.')) {
                fprintf(ImmediateFilePointer, "%c", BufferChar);
                while ((BufferChar = fgetc(FilePointer)) != '\n') {
                    fprintf(ImmediateFilePointer, "%c", BufferChar);
                }
                fprintf(ImmediateFilePointer, "%c", BufferChar);
                CommentFlag = 1;
                break;
            }
            else if ((BufferChar == ' ' || BufferChar == '\t') && CharacterFlag == 0) {
                // 처음부터 공백이 들어온 경우
                if (Flag == 0) {
                    Flag = 1;
                }
                // Opcode를 모두 읽고 공백이 들어온 경우
                else if (OpcodeMnemonicCounter && (Flag == 1)) {
                    Flag = 2;
                }
                continue;
            }
            else if (BufferChar == EOF) {
                break;
            }
            else {
                if (BufferChar == '\'' && Flag == 2) {
                    if (BeforeChar == 'C' && CharacterFlag == 0) {
                        CharacterFlag = 1;
                    }
                    else if (CharacterFlag == 1) {
                        CharacterFlag = 0;
                    }
                }
                BeforeChar = BufferChar;
                switch (Flag) {
                case 0:
                    Label = (char*)realloc(Label, sizeof(char) * (LabelCounter + 2));
                    Label[LabelCounter++] = BufferChar;
                    break;
                case 1:
                    OpcodeMnemonic = (char*)realloc(OpcodeMnemonic, sizeof(char) * (OpcodeMnemonicCounter + 2));
                    OpcodeMnemonic[OpcodeMnemonicCounter++] = BufferChar;
                    break;
                case 2:
                    Operand = (char*)realloc(Operand, sizeof(char) * (OperandCounter + 2));
                    Operand[OperandCounter++] = BufferChar;
                    break;
                }
            }
        }

        if (CommentFlag) {
            CommentFlag = 0;
            continue;
        }

        Label[LabelCounter] = OpcodeMnemonic[OpcodeMnemonicCounter] = Operand[OperandCounter] = '\0';

        RemoveBlank(&Label);
        RemoveBlank(&OpcodeMnemonic);
        RemoveBlank(&Operand);

        //printf("Label: [%s] Opcode: [%s] Operand: [%s]\n", Label, OpcodeMnemonic, Operand);

        if (OpcodeMnemonic != NULL) {
            // 첫 번째 line의 opcode가 'START'이면
            if (StartFlag == 0) {
                if (strcmp(OpcodeMnemonic, "START") == 0) {
                    if (Operand != NULL) {
                        if ((StartingAddress = HexStringToDec(Operand, Integer)) == -1) {
                            printf("[Line: %d] A starting address operand is not valid.\n", LineNumber);
                            break;
                        }
                        LocationCounter = StartingAddress;
                    }
                    else {
                        LocationCounter = 0;
                    }
                }
                // 첫 번? 줄의 OPCODE가 START가 아닌 경우
                else {
                    ErrorFlag = 1;
                    printf("[Line %d(%dth line)] An opcode of the first line is not 'START'.\n", LineNumber, LineNumber / 5);
                    break;
                }
            }
            HASH *FoundHash = NULL;
            switch (SearchOpcode(OpcodeMnemonic, &FoundHash)) {
                // Opcode Table에서 Opcode를 찾으면
            case 0:
                if (strcmp(FoundHash->Format, "1") == 0) {
                    AddLocationCounter = 1;
                }
                else if (strcmp(FoundHash->Format, "2") == 0) {
                    AddLocationCounter = 2;
                }
                else if (strcmp(FoundHash->Format, "3/4") == 0) {
                    if (OpcodeMnemonic[0] != '+') {
                        AddLocationCounter = 3;
                    }
                    else {
                        AddLocationCounter = 4;
                    }
                }
                // Opcode에 맞는 foramt이 없는 경우
                else {
                    ErrorFlag = 1;
                    printf("[Line %d(%dth line)] There is no valid format that is associated with the opcode.\n", LineNumber, LineNumber / 5);
                    break;
                }
                break;
                // Opcode Table에서 Opcode를 찾지 못하면
            case 1:
                if (Operand != NULL) {
                    if (strcmp(OpcodeMnemonic, "WORD") == 0) {
                        AddLocationCounter = 3;
                    }
                    else if (strcmp(OpcodeMnemonic, "RESW") == 0) {
                        int OperandToIntegerValue;
                        if ((OperandToIntegerValue = DecStringToDec(Operand, Integer)) >= 0) {
                            AddLocationCounter = 3 * OperandToIntegerValue;
                        }
                        else {
                            ErrorFlag = 1;
                            printf("[Line %d(%dth line)] Please check the operand of 'RESW'. The valid range is unsigned integer.\n", LineNumber, LineNumber / 5);
                            break;
                        }
                    }
                    else if (strcmp(OpcodeMnemonic, "RESB") == 0) {
                        int OperandToIntegerValue;
                        if ((OperandToIntegerValue = DecStringToDec(Operand, Integer)) >= 0) {
                            AddLocationCounter = OperandToIntegerValue;
                        }
                        else {
                            ErrorFlag = 1;
                            printf("[Line %d(%dth line)] Please check the operand of 'RESB'. The valid range is unsigned integer.\n", LineNumber, LineNumber / 5);
                            break;
                        }
                    }
                    else if (strcmp(OpcodeMnemonic, "BYTE") == 0) {
                        if (Operand[1] == '\'') {
                            int DigitNumber = 0;
                            for (int i = 2; Operand[i] != '\''; i++) {
                                DigitNumber++;
                            }
                            if (Operand[0] == 'X') {
                                if (DigitNumber > 60) {
                                    ErrorFlag = 1;
                                    printf("[Line %d(%dth line)] The length of byte you want to write is too long. ", LineNumber, LineNumber / 5);
                                    printf("It should be shorter than 60 digits.\n");
                                    break;
                                }
                                AddLocationCounter = (DigitNumber + 1) / 2;
                            }
                            else if (Operand[0] == 'C') {
                                if (DigitNumber > 30) {
                                    ErrorFlag = 1;
                                    printf("[Line %d(%dth line)] The length of byte you want to write is too long. ", LineNumber, LineNumber / 5);
                                    printf("It should be shorter than 30 characters.\n");
                                    break;
                                }
                                AddLocationCounter = DigitNumber;
                            }
                        }
                    }
                    else if (!strcmp(OpcodeMnemonic, "END") || !strcmp(OpcodeMnemonic, "BASE") || !strcmp(OpcodeMnemonic, "NOBASE")) {
                        NotAddressFlag = 1;
                    }
                    else if (!strcmp(OpcodeMnemonic, "START")) {
                        if (StartFlag == 1) {
                            ErrorFlag = 1;
                            printf("[Line %d(%dth line)] 'START' opcode appears twice in the file.\n", LineNumber, LineNumber / 5);
                        }
                        else {
                            StartFlag = 1;
                            NowStartFlag = 1;
                        }
                    }
                    else {
                        ErrorFlag = 1;
                        printf("[Line %d(%dth line)] There is no corresponding opcode or directive.\n", LineNumber, LineNumber / 5);
                        break;
                    }

                }
                else {
                    if (!strcmp(OpcodeMnemonic, "START")) {
                        if (StartFlag == 1) {
                            ErrorFlag = 1;
                            printf("[Line %d(%dth line)] 'START' opcode appears twice in the file.\n", LineNumber, LineNumber / 5);
                        }
                        else {
                            StartFlag = 1;
                            NowStartFlag = 1;
                        }
                        break;
                    }
                    else if (!strcmp(OpcodeMnemonic, "END")) {
                        NotAddressFlag = 1;
                    }
                    else {
                        ErrorFlag = 1;
                        printf("[Line %d(%dth line)] The opcode or directive should have an operand.\n", LineNumber, LineNumber / 5);
                        break;
                    }
                }
                break;
            default:
                break;
            }
            if (ErrorFlag == 1) {
                break;
            }
            if (Label != NULL && !NowStartFlag) {
                SYMBOL *TempPointer = NULL;
                // Label을 Symbol Table에서 찾으면
                if (SearchSymbolTable(Label, &TempPointer) == 0) {
                    ErrorFlag = 1;
                    printf("[Line %d(%dth line)] A label has already existed in the symbol table.\n", LineNumber, LineNumber / 5);
                    break;
                }
                // Label을 Symbol Table에서 찾지 못하면
                else if (SearchSymbolTable(Label, &TempPointer) == 1) {
                    InsertSymbolTable(Label, LocationCounter);
                }
            }
            if (strcmp(OpcodeMnemonic, "END") == 0) {
                if (EndFlag == 0) {
                    EndFlag = 1;
                }
                else {
                    ErrorFlag = 1;
                    printf("[Line %d(%dth line)] 'END' opcode appears twice in the file.\n", LineNumber, LineNumber / 5);
                    break;
                }
            }
            if (NowStartFlag) {
                NowStartFlag = 0;
            }
        }

        if (OpcodeMnemonicCounter) {
            if (!NotAddressFlag) {
                fprintf(ImmediateFilePointer, "%04X\t", LocationCounter & 0xFFFF);
            }
            else {
                fprintf(ImmediateFilePointer, "\t");
            }
            if (Label != NULL) {
                fprintf(ImmediateFilePointer, "%s\t", Label);
            }
            else {
                fprintf(ImmediateFilePointer, "\t");
            }
            if (OpcodeMnemonic != NULL) {
                fprintf(ImmediateFilePointer, "%s\t", OpcodeMnemonic);
            }
            else {
                fprintf(ImmediateFilePointer, "\t");
            }
            if (Operand != NULL) {
                fprintf(ImmediateFilePointer, "%s\t", Operand);
            }
            else {
                fprintf(ImmediateFilePointer, "\t");
            }
            fprintf(ImmediateFilePointer, "\n");
        }
        LocationCounter += AddLocationCounter;

        LabelCounter = AddLocationCounter = OpcodeMnemonicCounter = OperandCounter = 0;
        Flag = CharacterFlag = CommentFlag = 0, NotAddressFlag = 0;

        if (BufferChar == EOF) {
            if (EndFlag == 0) {
                ErrorFlag = 1;
                printf("[Line %d(%dth line)] There is no 'END' operand.\n", LineNumber, LineNumber / 5);
            }
            break;
        }
    }

    ProgramLength = LocationCounter - (StartAddress = StartingAddress);

    fclose(FilePointer);
    fclose(ImmediateFilePointer);

    if (ErrorFlag == 1) {
        DeleteSymbolTable();
        remove(ImmediateFileName);
        remove(ListFileName);
        remove(ObjectFileName);
    }

    if (Label != NULL) {
        free(Label);
    }
    if (OpcodeMnemonic != NULL) {
        free(OpcodeMnemonic);
    }
    if (Operand != NULL) {
        free(Operand);
    }
    if (FileName != NULL) {
        free(FileName);
    }
    if (ImmediateFileName != NULL) {
        free(ImmediateFileName);
    }
    if (ListFileName != NULL) {
        free(ListFileName);
    }
    if (ObjectFileName != NULL) {
        free(ObjectFileName);
    }

    if (ErrorFlag == 1) {
        return -1;
    }

    return 0;
}