#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h> // 리눅스에서 io.h 대신 사용
//#include <io.h>   // 비주얼스튜디오에서 unistd.h 대신 사용
#include <errno.h>

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
#define Type 13

#define Word    100
#define Character   101
#define Integer     102
#define Hexadecimal 103
#define ForHexAddress  104
#define ForProgramCounter  105

#define IndirectAddressing 200
#define ImmediateAddressing 201

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


extern NODE *FirstNode;
extern HASH HashTable[20]; // Hash Table 선언
extern SYMBOL SymbolTable[20]; // Symbol Table 선언

extern char DumpMemory[0x100000];
extern int LastAddress;
extern int StartAddress;
extern int ProgramLength;

extern void RemoveBlank(char **Parameter);
extern int HexStringToDec(char *Parameter, short DataType);
extern int DecStringToDec(char *Parameter, short Purpose);
extern int ReadShellCommand(char **InputShellCommand, char **Parameter1, char **Parameter2, char** Parameter3);
extern void HelpCommand();
extern void DirCommand();
extern void RecordPreviousCommand(char* InputShellCommand);
extern void HistoryCommand();
extern void DeleteHistory();
extern int DumpCommand(char *Start, char *End);
extern int EditCommand(char *Address, char *Value);
extern int FillCommand(char *Start, char *End, char *Value);
extern int ResetCommand();
extern int HashFunction(char *Value);
extern int OpcodeCommand(char *Mnemonic);
extern int SearchOpcode(char *Mnemonic, HASH** TempPointer);
extern int OpcodeListCommand();
extern void DeleteHashTable();
extern int CreateHashTable();
extern int InitializeSymbolTable();
extern int InsertSymbolTable(char *Label, int LocationCounter);
extern void DeleteSymbolTable();
extern int SearchSymbolTable(char *Label, SYMBOL **TempPointer);
extern int PrintSymbolTable();
extern int FindRegisterNumber(char *Register);
extern int AssemblePassOne(char *AssembleFileName);
extern int AssemblePassTwo(char *AssembleFileName);

#endif