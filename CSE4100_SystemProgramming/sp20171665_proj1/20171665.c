#include "20171665.h"

//#include <crtdbg.h> // 메모리 누수 확인

/*------------------------------------------------------------------------------------*/
/*함수 : RemoveBlank(char **Parameter)*/
/*목적 : 전달받은 Parameter에서 공백을 제거한다.
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void RemoveBlank(char **Parameter) {
    int Index = 0, TempIndex = 0;

    while ((*Parameter)[TempIndex] == ' ' || (*Parameter)[TempIndex] == '\t') {
        TempIndex++;
    }

    while ((*Parameter)[TempIndex] != '\0') {
        (*Parameter)[Index++] = (*Parameter)[TempIndex++];
    }

    if (!Index & *Parameter != NULL) {
        free(*Parameter);
        *Parameter = NULL;
    }
    else {
        (*Parameter)[Index] = '\0';
    }
    return;
}

/*------------------------------------------------------------------------------------*/
/*함수 : HexStringToDec(char *Parameter, short DataType)*/
/*목적 : 전달받은 Parameter에서 16진수로 된 string을 10진수의 integer 값으로 바꾼다.
/*리턴값 : 16진수를 10진수의 integer로 변환한 값을 반환한다. 입력한 값이 16진수가 아니거나 변환하고자 하는 data type의 범위를 초과할 경우 -1을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int HexStringToDec(char *Parameter, short DataType) {
    long Buffer = -1;
    char *EndPointer = NULL;
    errno = 0;

    Buffer = strtol(Parameter, &EndPointer, 16);
    if (EndPointer == Parameter || *EndPointer != '\0') {
        printf("No digits were found.\n");
        return -1;
    }
    if ((Buffer == 2147483647L || Buffer == -2147483647L - 1) && errno == ERANGE) {
        printf("The value you insert is invalid. Please write the valid value.\n");
        return -1;
    }
    if (!(Buffer >= 0x00 && Buffer <= 0xFF) && DataType == Character) {
        printf("The value you insert is out of boundary. (Boundary: 0x00 ~ 0xFF)\n");
        return -1;
    }

    return (int)Buffer;
}

/*------------------------------------------------------------------------------------*/
/*함수 : ReadShellCommand(char **InputShellCommand, char **Parameter1, char **Parameter2, char** Parameter3)*/
/*목적 : 전달받은 Command와 Parameter에 따라 유효한 명령을 수행할 수 있도록 한다.
/*리턴값 : 유효하지 않을 경우 0을 반환하고, 유효할 경우 입력받은 명령에 따라서 Header 파일에 정의된 명령값들을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int ReadShellCommand(char **InputShellCommand, char **Parameter1, char **Parameter2, char** Parameter3) {
    int BufferCounter = 0, ResultValue = 0;
    short Flag = 0, Invalid = 0;
    int CommandCounter = 0, ParameterCounter1 = 0, ParameterCounter2 = 0, ParameterCounter3 = 0;
    char BufferChar = EOF, BeforeChar = EOF, *BufferCommand = NULL;

    printf("sicsim> ");

    BufferCommand = (char*)realloc(BufferCommand, sizeof(char) * (CommandCounter + 2));
    *Parameter1 = (char*)realloc(*Parameter1, sizeof(char) * (ParameterCounter1 + 2));
    *Parameter2 = (char*)realloc(*Parameter2, sizeof(char) * (ParameterCounter2 + 2));
    *Parameter3 = (char*)realloc(*Parameter3, sizeof(char) * (ParameterCounter3 + 2));

    while ((BufferChar = getchar()) != '\n' && BufferChar != EOF) {
        *InputShellCommand = realloc(*InputShellCommand, sizeof(char) * (BufferCounter + 2));
        (*InputShellCommand)[BufferCounter++] = BufferChar;
        if (BufferChar == ' ' || BufferChar == '\t') {
            // 명령만 입력받은 상태에서 공백 또는 Tab이 입력된 경우
            if (CommandCounter && Flag == 0) {
                Flag = 1;
            }
            BeforeChar = BufferChar;
            continue;
        }
        else if (BufferChar == ',') {
            // Parameter를 한 개 입력받은 상태에서 ','가 입력된 경우
            if (ParameterCounter1 && Flag == 1) {
                Flag = 2;
            }
            // Parameter를 두 개 입력받은 상태에서 ','가 입력된 경우
            else if (ParameterCounter2 && Flag == 2) {
                Flag = 3;
            }
            // Parameter를 세 개 입력받은 후 또는 Command가 입력된 후 ','가 입력된 경우
            else if ((ParameterCounter3 && Flag == 3) || (!ParameterCounter1 && Flag == 1) || (Flag == 0)) {
                Invalid = 1;
            }
            BeforeChar = BufferChar;
            continue;
        }
        else if (BeforeChar == ' ') {
            // Parameter에서 띄어쓰기 처리가 된 경우
            if ((ParameterCounter1 && Flag == 1) || (ParameterCounter2 && Flag == 2) || (ParameterCounter3 && Flag == 3)) {
                Invalid = 1;
            }
        }
        switch (Flag) {
        case 0:
            BufferCommand = (char*)realloc(BufferCommand, sizeof(char) * (CommandCounter + 2));
            BufferCommand[CommandCounter++] = BufferChar;
            break;
        case 1:
            *Parameter1 = (char*)realloc(*Parameter1, sizeof(char) * (ParameterCounter1 + 2));
            (*Parameter1)[ParameterCounter1++] = BufferChar;
            break;
        case 2:
            *Parameter2 = (char*)realloc(*Parameter2, sizeof(char) * (ParameterCounter2 + 2));
            (*Parameter2)[ParameterCounter2++] = BufferChar;
            break;
        case 3:
            *Parameter3 = (char*)realloc(*Parameter3, sizeof(char) * (ParameterCounter3 + 2));
            (*Parameter3)[ParameterCounter3++] = BufferChar;
            break;
        default:
            break;
        }
        BeforeChar = BufferChar;
    }

	if ((*InputShellCommand) != NULL) {	
		BufferCommand[CommandCounter] = (*Parameter1)[ParameterCounter1] = (*Parameter2)[ParameterCounter2] = (*Parameter3)[ParameterCounter3] = (*InputShellCommand)[BufferCounter] = '\0';
	}

    RemoveBlank(Parameter1);
    RemoveBlank(Parameter2);
    RemoveBlank(Parameter3);

    if (Invalid != 1) {
        // 입력받은 Command에서 Parameter가 하나도 없는 경우
        if (*Parameter1 == NULL) {
            if (!strcmp(BufferCommand, "h") || !strcmp(BufferCommand, "help")) {
                ResultValue = Help;
            }
            else if (!strcmp(BufferCommand, "d") || !strcmp(BufferCommand, "dir")) {
                ResultValue = Dir;
            }
            else if (!strcmp(BufferCommand, "q") || !strcmp(BufferCommand, "quit")) {
                ResultValue = Quit;
            }
            else if (!strcmp(BufferCommand, "hi") || !strcmp(BufferCommand, "history")) {
                ResultValue = History;
            }
            else if (!strcmp(BufferCommand, "du") || !strcmp(BufferCommand, "dump")) {
                ResultValue = Dump;
            }
            else if (!strcmp(BufferCommand, "reset")) {
                ResultValue = Reset;
            }
            else if (!strcmp(BufferCommand, "opcodelist")) {
                ResultValue = OpcodeList;
            }
        }
        //Parameter가 최소 한 개 이상인 경우
        else {
            if ((!strcmp(BufferCommand, "du") || !strcmp(BufferCommand, "dump")) && Flag < 3) {
                ResultValue = Dump;
            }
            else if ((!strcmp(BufferCommand, "e") || !strcmp(BufferCommand, "edit")) && Flag < 3) {
                ResultValue = Edit;
            }
            else if (!strcmp(BufferCommand, "f") || !strcmp(BufferCommand, "fill")) {
                ResultValue = Fill;
            }
            else if (!strcmp(BufferCommand, "opcode") && Flag < 2) {
                ResultValue = Opcode;
            }
        }
    }

    if (BufferCommand != NULL) {
        free(BufferCommand);
    }

    return ResultValue;
}

/*------------------------------------------------------------------------------------*/
/*함수 : HelpCommand()*/
/*목적 : 사용자가 어떠한 Command와 그에 맞는 형식으로 Input을 입력해야 하는지를 화면에 출력한다.
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void HelpCommand() {
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp][start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
}

/*------------------------------------------------------------------------------------*/
/*함수 : DirCommand()*/
/*목적 : 현재 실행 파일이 위치하고 있는 디렉토리 내의 모든 파일과 폴더를 출력한다.
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void DirCommand() {
    DIR *DirectoryInfo = opendir(".");
    struct dirent *DirectoryEntry;
    struct stat BufferEntry;

    if (DirectoryInfo != NULL) {
        int Counter = 1;
        while ((DirectoryEntry = readdir(DirectoryInfo)) != NULL) {
	        if (Counter % 3 == 1) {
	    	    printf("\n");
	        }
	        else {
		        printf("\t");
	        }
            if (stat(DirectoryEntry->d_name, &BufferEntry) != -1) {
                if (S_ISDIR(BufferEntry.st_mode)) {
                    printf("%s/", DirectoryEntry->d_name);
                }
                if (S_ISREG(BufferEntry.st_mode)) {
                    printf("%s", DirectoryEntry->d_name);
                    if (BufferEntry.st_mode & S_IXUSR) {
                        printf("*");
                    }
                }
            }
            Counter++;
        }
	    printf("\n");
        closedir(DirectoryInfo);
    }
    else {
        printf("An error occurs while reading directory.\n");
    }
}

/*------------------------------------------------------------------------------------*/
/*함수 : RecordPreviousCommand()*/
/*목적 : 사용자가 입력한 Valid Command들을 Linked List에 기록한다.
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void RecordPreviousCommand(char* InputShellCommand) {
    NODE *TempNode = NULL, *PreviousNode = NULL;

    TempNode = (NODE*)malloc(sizeof(NODE));
    TempNode->Value = (char*)malloc(sizeof(char) * strlen(InputShellCommand)+ 1);
    TempNode->Next = NULL;
    strcpy(TempNode->Value, InputShellCommand);

    if (FirstNode == NULL) {
        FirstNode = TempNode;
    }
    else {
        PreviousNode = FirstNode;
        while (PreviousNode->Next != NULL) {
            PreviousNode = PreviousNode->Next;
        }
        PreviousNode->Next = TempNode;
    }
}

/*------------------------------------------------------------------------------------*/
/*함수 : HistoryCommand()*/
/*목적 : RecordPreviousCommand에 의해 저장된 사용자의 이전 Command들을 모두 출력한다.
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void HistoryCommand() {
    NODE *TempNode = FirstNode;
    int NodeIndex = 0;

    if (TempNode == NULL) {
        printf("No Command Recorded!\n");
    }
    else {
        while (TempNode != NULL) {
            printf("%-5d  %s\n", (++NodeIndex), TempNode->Value);
            TempNode = TempNode->Next;
        }
    }
}

/*------------------------------------------------------------------------------------*/
/*함수 : DeleteHistory()*/
/*목적 : Quit Command를 통해 프로그램을 종료할 때, 저장된 Command들을 모두 free하여 메모리 해제한다.
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void DeleteHistory() {
    NODE *TempNode = FirstNode, *DeleteNode = NULL;
    if (TempNode != NULL) {
        while (TempNode != NULL) {
            DeleteNode = TempNode;
            TempNode = TempNode->Next;
            free(DeleteNode->Value);
            free(DeleteNode);
        }
    }
    return;
}

/*------------------------------------------------------------------------------------*/
/*함수 : DumpCommand(char *Start, char *End)*/
/*목적 : 사전에 설정된 메모리(1MB)에서 사용자가 입력한 Parameter에 대응되는 주소 범위의 1 Byte 메모리 value를 출력한다.
/*리턴값 : 정상적인 Parameter가 입력이 들어오고 정확히 수행되면 0을 반환하고, 그렇지 않으면 -1을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int DumpCommand(char *Start, char *End) {
    int i = 0, j;
    int Counter, BufferStartParameter, BufferEndParameter;
    char Buffer;

    if (Start != NULL) {
        if ((BufferStartParameter = HexStringToDec(Start, Integer)) == -1) {
			printf("Please check the [start] address.\n");
            return -1;
        }
        if (!(BufferStartParameter >= 0 && BufferStartParameter <= 0xFFFFF)) {
            printf("[Start] address is out of the boundary.\n");
            return -1;
        }
        LastAddress = BufferStartParameter - 1;
    }
    LastAddress = (LastAddress + 1 <= 0xFFFFF) ? LastAddress : -1;

    if (End != NULL) {
        if ((BufferEndParameter = HexStringToDec(End, Integer)) == -1) {
			printf("Please check the [end] address.\n");
            return -1;
        }
        if (BufferEndParameter < BufferStartParameter) {
            printf("[End] address must be larger than [Start] Address.\n");
            return -1;
        }
        if (!(BufferEndParameter >= 0 && BufferEndParameter <= 0xFFFFF)) {
            printf("[End] address is out of the boundary.\n");
            return -1;
        }
        Counter = BufferEndParameter - LastAddress;
    }
    else {
        Counter = 160;
    }

    while (i < Counter) {
        if ((LastAddress + i + 1) > 0xFFFFF) {
            break;
        }
        if ((i++) == 0) {
            printf("%05X    ", ((LastAddress + i) / 16) * 16);
            j = 0;
            for (j = ((LastAddress + i) / 16) * 16; j < (LastAddress + i); j++) {
                printf("   ");
            }
        }
        else if ((LastAddress + i) % 16 == 0) {
            printf("    ;   ");
            for (j = 0; j < 16; j++) {
                if ((Buffer = DumpMemory[LastAddress + i + j - 16]) >= 32 && Buffer <= 126) {
                    printf("%c", Buffer);
                }
                else {
                    printf(".");
                }
            }
            printf("\n%05X    ", (LastAddress + i));
        }
        printf("%02X ", DumpMemory[LastAddress + i] & 0xFF);
    }

    if ((LastAddress + i + 1) % 16 == 0) {
        printf("    ;   ");
        for (j = ((LastAddress + i) / 16) * 16; j <= (LastAddress + i); j++) {
            if ((Buffer = DumpMemory[j]) >= 32 && Buffer <= 126) {
                printf("%c", Buffer);
            }
            else {
                printf(".");
            }
        }

    }
    else {
        for (j = (LastAddress + i + 1); j < ((LastAddress + i) / 16 + 1) * 16; j++) {
            printf("   ");
        }
        printf("    ;   ");
        for (j = ((LastAddress + i) / 16) * 16; j <= (LastAddress + i); j++) {
            if ((Buffer = DumpMemory[j]) >= 32 && Buffer <= 126) {
                printf("%c", Buffer);
            }
            else {
                printf(".");
            }
        }
        for (j = (LastAddress + i + 1); j < ((LastAddress + i) / 16 + 1) * 16; j++) {
            printf(".");
        }
    }
    printf("\n");
    LastAddress = LastAddress + i;
    return 0;
}

/*------------------------------------------------------------------------------------*/
/*함수 : EditCommand(char *Address, char *Value)*/
/*목적 : 메모리 주소 1 Byte와 수정하고자 하는 Value 값을 입력받아서 주소에 맞는 메모리 byte의 Value를 수정한다.
/*리턴값 : 정상적인 Parameter가 입력이 들어오고 정확히 수행되면 0을 반환하고, 그렇지 않으면 -1을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int EditCommand(char *Address, char *Value) {
    int BufferAddress = 0, BufferValue = 0;
    if (Address != NULL) {
        if ((BufferAddress = HexStringToDec(Address, Integer)) == -1) {
			printf("Please check the address.\n");
            return -1;
        }
        if (!(BufferAddress >= 0 && BufferAddress <= 0xFFFFF)) {
            printf("The address is out of the boundary. (Boundary: 0x00000 ~ 0xFFFFF)\n");
            return -1;
        }
        if (Value == NULL) {
            printf("Please write the valid value.\n");
            return -1;
        }
        if ((BufferValue = HexStringToDec(Value, Character)) == -1) {
			printf("Please check the value.\n");
            return -1;
        }
        DumpMemory[BufferAddress] = (char)BufferValue;
    }
    else {
        printf("Please write the valid address.\n");
        return -1;
    }
    return 0;
}

/*------------------------------------------------------------------------------------*/
/*함수 : FillCommand(char *Start, char *End, char *Value)*/
/*목적 : 메모리 주소 범위와 수정하고자 하는 Value 값을 입력받아서 주소 범위에 속하는 메모리 byte의 Value를 수정한다.
/*리턴값 : 정상적인 Parameter가 입력이 들어오고 정확히 수행되면 0을 반환하고, 그렇지 않으면 -1을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int FillCommand(char *Start, char *End, char *Value) {
    int BufferStartParameter = 0, BufferEndParameter = 0, Index = 0, BufferValue = 0;

    if (Start != NULL) {
        if ((BufferStartParameter = HexStringToDec(Start, Integer)) == -1) {
			printf("Please check the [start] address.\n");
            return -1;
        }
        if (!(BufferStartParameter >= 0 && BufferStartParameter <= 0xFFFFF)) {
            printf("The [start] address is out of the boundary. (Boundary: 0x00000 ~ 0xFFFFF)\n");
            return -1;
        }
        if (End != NULL) {
            if ((BufferEndParameter = HexStringToDec(End, Integer)) == -1) {
				printf("Please check the [end] address.\n");
                return -1;
            }
            if (BufferEndParameter < BufferStartParameter) {
                printf("The [end] address should not be smaller than [start] address.\n");
                return -1;
            }
            if (!(BufferEndParameter >= 0 && BufferEndParameter <= 0xFFFFF)) {
                printf("The [end] address is out of the boundary. (Boundary: 0x00000 ~ 0xFFFFF)\n");
                return -1;
            }
        }
        if (Value == NULL) {
            printf("Please write the vaild value.\n");
            return -1;
        }
        if ((BufferValue = HexStringToDec(Value, Character)) == -1) {
			printf("Please check the value.\n");
            return -1;
        }
        for (Index = BufferStartParameter; Index <= BufferEndParameter; Index++) {
            DumpMemory[Index] = (char)BufferValue;
        }
    }
    return 0;
}

/*------------------------------------------------------------------------------------*/
/*함수 : ResetCommand()*/
/*목적 : Reset Command를 사용자로부터 입력받으면 메모리(1MB)의 모든 Value 값을 0으로 초기화한다.
/*리턴값 : 정상적으로 수행되면 0을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int ResetCommand() {
    memset(DumpMemory, 0, sizeof(DumpMemory));
    return 0;
}

/*------------------------------------------------------------------------------------*/
/*함수 : HashFunction()*/
/*목적 : Opcode Mnemonic을 일대일 함수를 통해 대응되는 uniqly identified한 함수값을 구한다.
/*리턴값 : Opcode Mnemonic과 일대일 대응되는 독자적인 Hash 값을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int HashFunction(char *Mnemonic) {
    int Index = 0, ReturnValue = 0;

    while (Mnemonic[Index] != '\0') {
        ReturnValue += (int)Mnemonic[Index++] % 19;
    }
    ReturnValue %= 20;

    return ReturnValue;
}

/*------------------------------------------------------------------------------------*/
/*함수 : OpcodeCommand()*/
/*목적 : HashFunction 함수를 통해 구한 Hash 값을 사용하여 Hash Table을 탐색해서 Mnemonic에 대응되는 opcode를 구한다.
/*리턴값 : Opcode Mnemonic과 일대일 대응되는 opcode를 반환한다.*/
/*------------------------------------------------------------------------------------*/
int OpcodeCommand(char *Mnemonic) {
    int HashCode;
    HASH* TempHash = NULL;

    if (!((HashCode = HashFunction(Mnemonic)) >= 0 && HashCode < 20)) {
        printf("Error occurs while reading hash table.\n");
        return -1;
    }

    if ((TempHash = &(HashTable[HashCode])) == NULL) {
        printf("Error occurs while reading hash table.\n");
    }

    while ((strcmp(TempHash->Mnemonic, Mnemonic) != 0)) {
        if (TempHash->Next == NULL) {
            printf("There isn't an opcode you want to find.\n");
            return -1;
        }
        TempHash = TempHash->Next;
    }

    printf("opcode is %s\n", TempHash->Number);

    return 0;
}

/*------------------------------------------------------------------------------------*/
/*함수 : OpcodeListCommand()*/
/*목적 : HashFunction 함수에 의해 구성된 Hash Table의 형태를 출력한다.
/*리턴값 : Hash Table 탐색이 정상적으로 이루어지고 모든 opcode가 출력되면 0을 반환한다.*/
/*------------------------------------------------------------------------------------*/
int OpcodeListCommand() {
    int Index;
    HASH *TempHash = NULL;

    for (Index = 0; Index < 20; Index++) {
        printf("%d : ", Index);
        if (strcmp(HashTable[Index].Mnemonic, "")) {
            TempHash = &(HashTable[Index]);
            while (TempHash->Next != NULL) {
                printf("[%s, %s] -> ", TempHash->Mnemonic, TempHash->Number);
                    TempHash = TempHash->Next;
            }
            printf("[%s, %s]\n", TempHash->Mnemonic, TempHash->Number);
        }
        else {
            printf("\n");
        }
    }
    return 0;
}

/*------------------------------------------------------------------------------------*/
/*함수 : DeleteHashTable()*/
/*목적 : Quit Command를 통해 프로그램을 종료할 때 기존에 저장된 Hash Table를 free로 메모리 해제한다.
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void DeleteHashTable() {
    int Index;
    HASH *TempHash = NULL, *DeleteHash = NULL;

    for (Index = 0; Index < 20; Index++) {
        if (strcmp(HashTable[Index].Mnemonic, "")) {
            TempHash = &(HashTable[Index]);
            if ((TempHash = TempHash->Next) != NULL) {
                while (TempHash != NULL) {
                    DeleteHash = TempHash;
                    TempHash = TempHash->Next;
                    free(DeleteHash);
                }
            }
        }
    }

    return;
}

int CreateHashTable() {
    FILE *FilePointer = NULL;

    if ((FilePointer = fopen("opcode.txt", "r")) == NULL) {
        printf("An error occurs while opening 'opcode.txt' file.\n");
        return -1;
    }

    int Buffer;
    char BufferNumber[5] = { '\0' }, BufferMnemonic[10] = { '\0' }, BufferFormat[10] = { '\0' };

    while (fscanf(FilePointer, "%s  %s  %s\n", BufferNumber, BufferMnemonic, BufferFormat) != EOF) {

        if (!strcmp(HashTable[(Buffer = HashFunction(BufferMnemonic))].Mnemonic, "")) {
            strcpy(HashTable[Buffer].Number, BufferNumber);
            strcpy(HashTable[Buffer].Mnemonic, BufferMnemonic);
            strcpy(HashTable[Buffer].Format, BufferFormat);
            HashTable[Buffer].Next = NULL;
        }
        else {
            HASH *NewHash = NULL, *TempHash = &(HashTable[Buffer]);
            NewHash = (HASH*)malloc(sizeof(HASH));

            strcpy(NewHash->Number, BufferNumber);
            strcpy(NewHash->Mnemonic, BufferMnemonic);
            strcpy(NewHash->Format, BufferFormat);
            NewHash->Next = NULL;

            while (TempHash->Next != NULL) {
                TempHash = TempHash->Next;
            }
            TempHash->Next = NewHash;
        }
    }
    fclose(FilePointer);
    return 0;
}

/*프로그램 시작*/
int main() {
    char *InputShellCommand = NULL;
    char *Parameter1 = NULL, *Parameter2 = NULL, *Parameter3 = NULL;
    int ReadFlag = 0;

    if (CreateHashTable() == -1) {
        printf("Error occurs while creating hash table\n.");
        return -1;
    }

    while ((ReadFlag = ReadShellCommand(&InputShellCommand, &Parameter1, &Parameter2, &Parameter3)) != Quit) {
        if (ReadFlag != 0) {
            switch (ReadFlag) {
            case Help:
                RecordPreviousCommand(InputShellCommand);
                HelpCommand();
                break;
            case Dir:
                RecordPreviousCommand(InputShellCommand);
                DirCommand();
                break;
            case History:
                RecordPreviousCommand(InputShellCommand);
                HistoryCommand();
                break;
            case Dump:
                if (!DumpCommand(Parameter1, Parameter2)) {
                    RecordPreviousCommand(InputShellCommand);
                }
                break;
            case Edit:
                if (!EditCommand(Parameter1, Parameter2)) {
                    RecordPreviousCommand(InputShellCommand);
                }
                break;
            case Fill:
                if (!FillCommand(Parameter1, Parameter2, Parameter3)) {
                    RecordPreviousCommand(InputShellCommand);
                }
                break;
            case Reset:
                if (!ResetCommand()) {
                    RecordPreviousCommand(InputShellCommand);
                }
                break;
            case Opcode:
                if (!OpcodeCommand(Parameter1)) {
                    RecordPreviousCommand(InputShellCommand);
                }
                break;
            case OpcodeList:
                if (!OpcodeListCommand()) {
                    RecordPreviousCommand(InputShellCommand);
                }
                break;
            default:
                printf("Please write the vaild command. You can refer to the valid command list by executing 'help' command.\n");
                break;
            }
        }
        else {
            printf("Please write the vaild command. You can refer to the valid command list by executing 'help' command.\n");
        }
    }

    DeleteHistory();
    DeleteHashTable();

    if (InputShellCommand != NULL) {
        free(InputShellCommand);
    }
    if (Parameter1 != NULL) {
        free(Parameter1);
    }
    if (Parameter2 != NULL) {
        free(Parameter2);
    }
    if (Parameter3 != NULL) {
        free(Parameter3);
    }

    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // 메모리 누수 확인

    return 0;
}
