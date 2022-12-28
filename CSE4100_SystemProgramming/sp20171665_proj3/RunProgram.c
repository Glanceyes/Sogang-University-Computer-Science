#include "20171665.h"

#define LDA     0X00
#define LDB     0X68
#define LDT     0X74
#define LDCH    0X50
#define STA     0X0C
#define STX     0X10
#define STL     0X14
#define STCH    0X54
#define J       0X3C
#define JSUB    0X48
#define JLT     0X38
#define JEQ     0X30
#define RSUB    0X4C
#define COMP    0X28
#define COMPR   0XA0
#define CLEAR   0XB4
#define TIXR    0XB8
#define TD      0XE0
#define RD      0XD8
#define WD      0XDC

#define LessThan    0
#define Equal       1
#define GreaterThan 2

enum RegisterNumber {
    RegisterA, RegisterX, RegisterL, RegisterB, RegisterS, RegisterT, RegisterF, RegisterTemp, RegisterPC, RegisterSW
};

int Register[10] = { 0 };
int DebugFlag = 0;
int BreakPoint[0X100000] = { 0 };
int BreakPointSize = 0;
int BreakPointCheck = -1;
int BreakPointCheckFlag = 0;

int FindBreakPoint(int Address) {
    for (int i = 0; i < BreakPointSize; i++) {
        if (BreakPoint[i] == Address) {
            return 1;
        }
    }
    return 0;
}

void SetBreakPoint(int Address) {
    if (FindBreakPoint(Address)) {
        return;
    }
    
    BreakPoint[BreakPointSize++] = Address;
	
	/*
    for (int i = 0; i < BreakPointSize; i++) {
        for (int j = 0; j < i; j++) {
            if (BreakPoint[j] > BreakPoint[j + 1]) {
                int TempBreakPoint = BreakPoint[j];
                BreakPoint[j] = BreakPoint[j + 1];
                BreakPoint[j + 1] = TempBreakPoint;
            }
        }
    }
	*/
    printf("\n[ok] create breakpoint %05X\n\n", Address);
    return;
}

void PrintBreakPoint() {
    if (BreakPointSize) {
        printf("\nbreakpoints\n");
        printf("-----------\n");
        for (int i = 0; i < BreakPointSize; i++) {
            printf("%05X\n", BreakPoint[i]);
        }
        printf("\n");
    }
    else {
        printf("\nno breakpoints set.\n\n");
    }

    return;
}

void ClearAllBreakPoint() {
    for (int i = 0; i < BreakPointSize; i++) {
        BreakPoint[i] = 0;
    }
    BreakPointSize = 0;
    printf("\n[ok] clear all breakpoints\n\n");
    return;
}

void PrintRegister(int EndProgramAddress) {
    printf("A : %06X   X : %06X\n", Register[RegisterA], Register[RegisterX]);
    printf("L : %06X   PC: %06X\n", Register[RegisterL], Register[RegisterPC]);
    printf("B : %06X   S : %06X\n", Register[RegisterB], Register[RegisterS]);
    printf("T : %06X\n", Register[RegisterT]);

    if (EndProgramAddress == -1) {
        printf("End program\n");
    }
    else {
        printf("Stop at checkpoint[%05X]\n", EndProgramAddress);
    }
}

void ExecuteInstruction(int OpcodeValue, int FirstRegister, int SecondRegister, int NI, int TargetAddress) {
    int OperandValue = 0, TempValue;
    
    if (NI == 1) {
        OperandValue = TargetAddress;
    }
    else if (NI == 2) {
        int IndirectTargetAddress;
        IndirectTargetAddress = DumpMemory[TargetAddress] & 0XFF;
        IndirectTargetAddress = IndirectTargetAddress << 8;

        IndirectTargetAddress += DumpMemory[TargetAddress + 1] & 0XFF;
        IndirectTargetAddress = IndirectTargetAddress << 8;

        IndirectTargetAddress += DumpMemory[TargetAddress + 2] & 0XFF;
        TargetAddress = IndirectTargetAddress;

        OperandValue += DumpMemory[TargetAddress] & 0XFF;
        OperandValue = OperandValue << 8;

        OperandValue += DumpMemory[TargetAddress + 1] & 0XFF;
        OperandValue = OperandValue << 8;

        OperandValue += DumpMemory[TargetAddress + 2] & 0XFF;
    }
    else {
        OperandValue += DumpMemory[TargetAddress] & 0XFF;
        OperandValue = OperandValue << 8;

        OperandValue += DumpMemory[TargetAddress + 1] & 0XFF;
        OperandValue = OperandValue << 8;

        OperandValue += DumpMemory[TargetAddress + 2] & 0XFF;
    }

    if (OperandValue >= (int)pow(2, 23)) {
        OperandValue |= 0XFF000000;
    }

    switch (OpcodeValue) {
    case LDA:
        Register[RegisterA] = OperandValue;
        break;
    case LDB:
        Register[RegisterB] = OperandValue;
        break;
    case LDT:
        Register[RegisterT] = OperandValue;
        break;
    case LDCH:
        Register[RegisterA] = DumpMemory[TargetAddress] & 0XFF;
        break;
    case STA:
        TempValue = Register[RegisterA];

        DumpMemory[TargetAddress + 2] = (TempValue % 256) & 0XFF;
        TempValue /= 256;

        DumpMemory[TargetAddress + 1] = (TempValue % 256) & 0XFF;
        TempValue /= 256;

        DumpMemory[TargetAddress] = (TempValue % 256) & 0XFF;
        break;
    case STX:
        TempValue = Register[RegisterX];

        DumpMemory[TargetAddress + 2] = (TempValue % 256) & 0XFF;
        TempValue /= 256;

        DumpMemory[TargetAddress + 1] = (TempValue % 256) & 0XFF;
        TempValue /= 256;

        DumpMemory[TargetAddress] = (TempValue % 256) & 0XFF;
        break;
    case STL:
        TempValue = Register[RegisterL];

        DumpMemory[TargetAddress + 2] = (TempValue % 256) & 0XFF;
        TempValue /= 256;

        DumpMemory[TargetAddress + 1] = (TempValue % 256) & 0XFF;
        TempValue /= 256;

        DumpMemory[TargetAddress] = (TempValue % 256) & 0XFF;
        break;
    case STCH:
        TempValue = Register[RegisterA];
        DumpMemory[TargetAddress] = (TempValue % 256) & 0XFF;
        break;
    case J:
        Register[RegisterPC] = TargetAddress;
        break;
    case JSUB:
        Register[RegisterL] = Register[RegisterPC];
        Register[RegisterPC] = TargetAddress;
        break;
    case JLT:
        if (Register[RegisterSW] == LessThan) {
            Register[RegisterPC] = TargetAddress;
        }
        break;
    case JEQ:
        if (Register[RegisterSW] == Equal) {
            Register[RegisterPC] = TargetAddress;
        }
        break;
    case RSUB:
        Register[RegisterPC] = Register[RegisterL];
        break;
    case COMP:
        if (Register[RegisterA] < OperandValue) {
            Register[RegisterSW] = LessThan;
        }
        else if (Register[RegisterA] == OperandValue) {
            Register[RegisterSW] = Equal;
        }
        else {
            Register[RegisterSW] = GreaterThan;
        }
        break;
    case COMPR:
        if (Register[FirstRegister] < Register[SecondRegister]) {
            Register[RegisterSW] = LessThan;
        }
        else if (Register[FirstRegister] == Register[SecondRegister]) {
            Register[RegisterSW] = Equal;
        }
        else {
            Register[RegisterSW] = GreaterThan;
        }
        break;
    case CLEAR:
        Register[FirstRegister] = 0;
        break;
    case TIXR:
        Register[RegisterX]++;
        if (Register[RegisterX] < Register[FirstRegister]) {
            Register[RegisterSW] = LessThan;
        }
        else if (Register[RegisterX] == Register[FirstRegister]) {
            Register[RegisterSW] = Equal;
        }
        else {
            Register[RegisterSW] = GreaterThan;
        }
        break;
    case TD:
        Register[RegisterSW] = LessThan;
        break;
    case RD:
        Register[RegisterA] /= 0X100;
        Register[RegisterA] *= 0X100;
        Register[RegisterA] += 0X00;
        break;
    case WD:
        break;
    default:
        break;
    }
}

void XBPEValue(int XBPE, int *X, int *B, int *P, int *E) {
    if (XBPE % 2) {
        *E = 1;
    }
    else {
        *E = 0;
    }
    XBPE /= 2;

    if (XBPE % 2) {
        *P = 1;
    }
    else {
        *P = 0;
    }
    XBPE /= 2;

    if (XBPE % 2) {
        *B = 1;
    }
    else {
        *B = 0;
    }
    XBPE /= 2;

    if (XBPE % 2) {
        *X = 1;
    }
    else {
        *X = 0;
    }

    return;
}

void InitializeExecution() {
    for (int i = 0; i < 10; i++) {
        Register[i] = 0;
    }
    DebugFlag = 0;
	BreakPointCheck = -1;
}

int RunCommand() {
    int Result = 0;

    Result = RunProgram();
    if (!BreakPointCheckFlag && Result <= 0) {
        //Register[RegisterPC] = ExecutionAddress + ProgramTotalLength;
        PrintRegister(-1);
        InitializeExecution();
    }
    else if (BreakPointCheckFlag) {
        PrintRegister(Result * -1);
		BreakPointCheckFlag = 0;
    }
    else {
        PrintRegister(-1);
        printf("[Runtime error] Check it whether the invalid memory access or opcode occurs.\n");
        InitializeExecution();
        return -1;
        // 에러
    }

    return 0;
}

int BreakPointCommand(char *Parameter1) {
    if (Parameter1 != NULL) {
        if (!strcmp(Parameter1, "clear")) {
            ClearAllBreakPoint();
        }
        else {
            int BreakPoint = HexStringToDec(Parameter1, ForHexAddress);
            SetBreakPoint(BreakPoint);
        }
    }
    else {
        PrintBreakPoint();
    }

    return 0;
}

int RunProgram() {

    if (!DebugFlag) {
        Register[RegisterPC] = ExecutionAddress;
		Register[RegisterL] = ExecutionAddress + ProgramTotalLength;
    }

    while (Register[RegisterPC] < MaxAddress) {
		if (FindBreakPoint(Register[RegisterPC]) && BreakPointCheck != Register[RegisterPC]) {
			BreakPointCheck = Register[RegisterPC];
			BreakPointCheckFlag = 1;
			return -1 * Register[RegisterPC];
		}

        int FirstRegister = -1, SecondRegister = -1, TargetAddress = -1;
        //printf("PC: %05X\n", Register[RegisterPC] & 0XFFFFF);
        int OpcodeValue = (DumpMemory[Register[RegisterPC]++] & 0XFF);
        int X, B, P, E, NI = OpcodeValue % 4;

        X = B = P = E = 0;
        DebugFlag = 1;
        OpcodeValue -= NI;

        //printf("Opcode: %02X\n", OpcodeValue);

        HASH* FoundHash = NULL;
        if (!SearchOpcodeByValue(OpcodeValue, &FoundHash)) {
            if (!strcmp(FoundHash->Format, "1") && NI != 0) {
                // 에러
                return 1;
            }
            else if (!strcmp(FoundHash->Format, "2")) {
                //if (Register[RegisterPC] >= MaxAddress) {
                    // 에러
                //}
                int RegisterValue = DumpMemory[Register[RegisterPC]++];

                if (OpcodeValue % 4 != 0) {
                    // 에러
                    return 1;
                }
                FirstRegister = RegisterValue / 16;
                SecondRegister = RegisterValue % 16;
                if (FirstRegister > 9 || SecondRegister > 9) {
                    //에러
                    return 1;
                }

            }
            else if (!strcmp(FoundHash->Format, "3/4")) {
                //if (Register[RegisterPC] >= MaxAddress) {
                    // 에러
                //}
                int TempValue = (DumpMemory[Register[RegisterPC]++] & 0XFF);
                //printf("[2] PC: %05X\n", Register[RegisterPC] & 0XFFFFF);
                int FirstAddressByte = TempValue % 0X10;
                int XBPE = TempValue / 0X10;

                XBPEValue(XBPE, &X, &B, &P, &E);

                //if (Register[RegisterPC] >= MaxAddress) {
                    // 에러
                //}
                TargetAddress = (DumpMemory[Register[RegisterPC]++] & 0xFF);
                //printf("[3] PC: %05X\n", Register[RegisterPC] & 0XFFFFF);
                TargetAddress += (FirstAddressByte << 8);

                if (E) {
                    //if (Register[RegisterPC] >= MaxAddress) {
                        // 에러
                    //}
                    int ExtendValue = (DumpMemory[Register[RegisterPC]++] & 0XFF);
                    //printf("[4] PC: %05X\n", Register[RegisterPC] & 0XFFFFF);
                    TargetAddress = TargetAddress << 8;
                    TargetAddress += ExtendValue;

                    if (B || P) {
                        // 에러
                        return 1;
                    }
                }

                if (X) {
                    TargetAddress += Register[RegisterX];
                }

                if (!B && P) {
                    if (TargetAddress > 2047) {
                        TargetAddress |= 0XFFFFF000;
                    }
                    TargetAddress += Register[RegisterPC];
                }
                else if (B && !P) {
                    TargetAddress += Register[RegisterB];
                }

                /*
                if (TargetAddress < 0 || TargetAddress >= MaxAddress) {
                    // 에러
                    return 1;
                }
                */
                
                    
                if (NI == 2) {
					/*
                    if (DumpMemory[TargetAddress] < 0 || DumpMemory[TargetAddress] >= MaxAddress) {
                        // 에러
                        return 1;
                    }
					*/
                }
            }

            //BreakPointCheck = Register[RegisterPC];

            ExecuteInstruction(OpcodeValue, FirstRegister, SecondRegister, NI, TargetAddress);

            if (Register[RegisterPC] == ExecutionAddress + ProgramTotalLength) {
                break;
            }

            //if (FindBreakPoint(BreakPointCheck)) {
            //    return -1 * BreakPointCheck;
            //}
        }
        else {
            return 0;
        }
    }
    
    if (Register[RegisterPC] >= MaxAddress) {
        Register[RegisterPC] = 0;
    }

    return 0;
}