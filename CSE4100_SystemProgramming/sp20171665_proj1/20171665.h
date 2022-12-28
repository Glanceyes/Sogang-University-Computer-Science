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

#define Character   11
#define Integer     12

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

NODE *FirstNode = NULL;
HASH HashTable[20]; // Hash Table 선언
char DumpMemory[0x100000] = { 0 };
int LastAddress = -1;
