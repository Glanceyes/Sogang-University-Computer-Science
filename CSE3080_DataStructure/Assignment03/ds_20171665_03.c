#include<stdio.h>

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define RIGHT 0
#define LEFT 1

typedef enum { false = 0, true = 1 } boolean;

typedef struct threadedTree *threadedPointer;
typedef struct threadedTree {
    short int leftThread;
    threadedPointer leftChild;
    char data;
    threadedPointer rightChild;
    short int rightThread;
} threadedTree;

threadedPointer insucc(threadedPointer tree);
void tinorder(threadedPointer tree);
void insertRight(threadedPointer s, threadedPointer r);

void construct_tree(threadedPointer tree, FILE *fp);
void insertLeft(threadedPointer s, threadedPointer r);
void insert(threadedPointer s, char data, short int insertFlag);

threadedPointer inpredec(threadedPointer tree);
threadedPointer tinorderSearch(threadedPointer tree, char searchingData);
void tpreorder(threadedPointer tree);

void tinorder_free(threadedPointer tree);
threadedPointer insucc_free(threadedPointer tree);


int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    FILE *fp;

    /* initialize a head node */
    threadedPointer root;
    root = (threadedPointer)malloc(sizeof(threadedTree));
    root->leftThread = true;
    root->rightThread = false;
    root->leftChild = root->rightChild = root;
    root->data = '\0';

    /* construct a given tree */
    fp = fopen("input.txt", "r");
    construct_tree(root, fp);

    tinorder(root);
    printf("\n");
    tpreorder(root->leftChild);

    tinorder_free(root);
    return 0;
}

void construct_tree(threadedPointer tree, FILE *fp)
{
    char operation, data, direction;
    threadedPointer temp = tree;
    
    while (EOF != fscanf(fp, "%c ", &operation)) {
        if (operation == 'I') {
            fscanf(fp, "%c %c\n", &data, &direction);
            if (direction == 'L') insert(temp, data, 1);
            else if (direction == 'R') insert(temp, data, 0);
        }
        else if (operation == 'S') {
            fscanf(fp, "%c\n", &data);
            temp = tinorderSearch(temp, data);
        }
    }
}

threadedPointer insucc(threadedPointer tree)
{
    threadedPointer temp;
    temp = tree->rightChild;
    if (!tree->rightThread) {
        while (!temp->leftThread) {
            temp = temp->leftChild;
        }
    }
    return temp;
}

threadedPointer inpredec(threadedPointer tree)
{
    threadedPointer temp;
    temp = tree->leftChild;
    if (!tree->leftThread) {
        while (!temp->rightThread) {
            temp = temp->rightChild;
        }
    }
    return temp;
}

void tinorder(threadedPointer tree)
{
    threadedPointer temp = tree;
    for (;;) {
        temp = insucc(temp);
        if (temp == tree) break;
        printf("%3c", temp->data);
    }
}

void tpreorder(threadedPointer tree)
{
    threadedPointer temp = tree;
    for (;;) {
        printf("%3c", temp->data);
        if (!temp->leftThread) {
            temp = temp->leftChild;
        }
        else {
            while (temp->rightThread) {
                temp = temp->rightChild;
            }
            if (temp == temp->rightChild) break;
            temp = temp->rightChild;
        }
    }
    printf("\n");
}

threadedPointer tinorderSearch(threadedPointer tree, char searchingData)
{
    threadedPointer temp = tree;
    for (;;) {
        temp = insucc(temp);
        if (temp->data == searchingData) return temp;
        if (temp == tree) break;
    }
    return 0;
}

void insert(threadedPointer s, char data, short int insertFlag)
{
    // type your code
    threadedPointer child;
    child = (threadedPointer)malloc(sizeof(threadedTree));
    child->leftChild = child->rightChild = child;
    child->leftThread = child->rightThread = true;
    child->data = data;

    if(insertFlag == 0) insertRight(s, child);
    else if (insertFlag == 1) insertLeft(s, child);
}

void insertRight(threadedPointer s, threadedPointer r)
{
    threadedPointer temp;
    r->rightChild = s->rightChild;
    r->rightThread = s->rightThread;
    r->leftChild = s;
    s->rightChild = r;
    s->rightThread = false;
    if (!r->rightThread) {
        temp = insucc(r);
        temp->leftChild = r;
    }
}

void insertLeft(threadedPointer s, threadedPointer l)
{
    threadedPointer temp;
    l->leftChild = s->leftChild;
    l->leftThread = s->leftThread;
    l->rightChild = s;
    s->leftChild = l;
    s->leftThread = false;
    if (!l->leftThread) {
        temp = inpredec(l);
        temp->rightChild = l;
    }
}

threadedPointer insucc_free(threadedPointer tree)
{
    //동적할당된 것 free
    threadedPointer temp;
    temp = tree->rightChild;
    if (!tree->rightThread) {
        while (!temp->leftThread) {
            temp = temp->leftChild;
        }
    }
    if (tree->rightThread || tree->rightChild != tree)
        free(tree);
    return temp;
}

void tinorder_free(threadedPointer tree)
{
    threadedPointer temp = tree;
    for (;;) {
        temp = insucc_free(temp);
        if (temp == tree) {
            free(tree);
            break;
        }
    }
}