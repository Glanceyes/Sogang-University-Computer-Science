#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h> // 리눅스에서 io.h 대신 사용
//#include <io.h>   // 비주얼스튜디오에서 unistd.h 대신 사용
#include <errno.h>
#include <math.h>

#define Help        1
#define Dir         2
#define Quit        3
#define History     4
#define Dump        5
#define Edit        6
#define Fill        7
#define Reset       8
#define Opcode      9
#define OpcodeList  10
#define Assemble    11
#define SymbolCommand 12
#define Type        13
#define Progaddr    14
#define Loader      15
#define Run         16
#define BP          17

#define Word    100
#define Character   101
#define Integer     102
#define Hexadecimal 103
#define ForHexAddress  104
#define ForProgramCounter  105

#define IndirectAddressing 200
#define ImmediateAddressing 201

#define IsControlSection 300
#define IsSymbol 301

#define MaxAddress  0X100000

typedef struct Node {
    char *Value;
    struct Node* Next;
}NODE;

typedef struct Hash {
    char Number[5];
    char Mnemonic[10];
    char Format[10];
    struct Hash* Next;
}HASH;

typedef struct Symbol {
    char *LabelName;
    int LabelAddress;
    short LabelFlag;
    struct Symbol* Next;
}SYMBOL;

typedef struct ExternalSymbol {
    char *SymbolName;
    int SymbolAddress;
    int ControlSectionLength;
    int OrderNumber;
    int ControlSectionOrNot;
    struct ExternalSymbol* Next;
}EXTSYMBOL;

extern NODE *FirstNode;
extern HASH HashTable[20]; // Hash Table 선언
extern SYMBOL SymbolTable[20]; // Symbol Table 선언
extern EXTSYMBOL ESTAB[20]; // External Symbol Table 선언

extern char DumpMemory[MaxAddress];
extern int LastAddress;
extern int StartAddress;
extern int ProgramLength;
extern int ProgramStartAddress;
extern int ExecutionAddress;
extern int ProgramTotalLength;

extern int Register[10];
extern int DebugFlag;
extern int BreakPoint[0X100000];
extern int BreakPointSize;
extern int BreakPointCheck;
extern int BreakPointCheckFlag;

extern void RemoveBlank(char **Parameter);
extern int HexStringToDec(char *Parameter, short DataType);
extern int HexCharToDec(char HexCharacter);
extern int DecStringToDec(char *Parameter, short Purpose);

extern int ReadShellCommand(char **InputShellCommand, char **Parameter1, char **Parameter2, char** Parameter3);
extern void HelpCommand();
extern void DirCommand();
extern void RecordPreviousCommand(char* InputShellCommand);
extern void HistoryCommand();
extern void DeleteHistory();
extern int ProgaddrCommand(char* InputProgramAddress);
extern int DumpCommand(char *Start, char *End);
extern int EditCommand(char *Address, char *Value);
extern int FillCommand(char *Start, char *End, char *Value);
extern int ResetCommand();

extern int HashFunction(char *Value);

extern int OpcodeCommand(char *Mnemonic);
extern int SearchOpcode(char *Mnemonic, HASH** TempPointer);
extern int SearchOpcodeByValue(int OpcodeValue, HASH** TempPointer);
extern int OpcodeListCommand();

extern void DeleteHashTable();
extern int CreateHashTable();

extern int InitializeSymbolTable();
extern int InsertSymbolTable(char *Label, int LocationCounter);
extern void DeleteSymbolTable();
extern int SearchSymbolTable(char *Label, SYMBOL **TempPointer);
extern int PrintSymbolTable();

extern void TrimRightBlank(char *String);
extern int InitializeESTAB();
extern int InsertESTAB(char *SymbolName, int LocationCounter, int ControlSectionOrNot, int OrderNumber, int ControlSectionLength);
extern void DeleteESTAB();
extern int SearchESTAB(char *SymbolName, EXTSYMBOL **TempPointer);
extern int PrintESTAB();

extern int LoadObjectCode(int ModifiedLengthValue, int ModifiedOddLengthFlag, int CSADDR, int AddressValue, char OperatorCharacter, EXTSYMBOL *TempSymbol, int AddressBlankFlag);

extern int FindRegisterNumber(char *Register);

extern int AssemblePassOne(char *AssembleFileName);
extern int AssemblePassTwo(char *AssembleFileName);

extern int LoadPassOne(char *FileName1, char *FileName2, char *FileName3);
extern int LoadPassTwo(char *FileName1, char *FileName2, char *FileName3);

extern int RunProgram();
extern int RunCommand();
extern int BreakPointCommand(char *Parameter1);
extern int FindBreakPoint(int Address);
extern void SetBreakPoint(int Address);
extern void PrintBreakPoint();
extern void ClearAllBreakPoint();
extern void PrintRegister(int EndProgramAddress);
extern void ExecuteInstruction(int OpcodeValue, int FirstRegister, int SecondRegister, int NI, int TargetAddress);
extern void XBPEValue(int XBPE, int *X, int *B, int *P, int *E);
extern void InitializeExecution();

#endif