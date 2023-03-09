#include<stdio.h>
#include<stdlib.h>
#include <crtdbg.h>
#define MAX_TERMS 100

typedef struct treeNode* treePtr;
typedef struct treeNode {
    treePtr lchild;
    int data;
    treePtr rchild;
} treeNode;

void inorder(treePtr ptr){
    if (ptr) {
        inorder(ptr->lchild);
        printf("%d\n", ptr->data);
        inorder(ptr->rchild);
    }
}
void preorder(treePtr ptr) {
    if (ptr) {
        printf("%d\n", ptr->data);
        preorder(ptr->lchild);
        preorder(ptr->rchild);
    }
}
treePtr modifiedSearch(treePtr tree, int k) {
    treePtr temp = NULL;
    while (tree) {
        temp = tree;
        if (k == tree->data) return NULL;
        if (k < (tree->data)) tree = tree->lchild;
        else tree = tree->rchild;
    }
    return temp;
}
void insert(treePtr *node, int k) {
    treePtr ptr, temp = modifiedSearch(*node, k);
    if (temp || !(*node)) {
        ptr = (treePtr)malloc(sizeof(treeNode));
        ptr->data = k;
        ptr->lchild = ptr->rchild = NULL;
        if (*node) {
            if (k < (temp->data)) temp->lchild = ptr;
            else temp->rchild = ptr;
        }
        else *node = ptr;
    }
}

void treefree(treePtr node) {
    if (node != NULL) {
        treefree(node->rchild);
        treefree(node->lchild);
        free(node);
    }
}

void main() {
    int i, n, A[MAX_TERMS];
    treePtr tree = NULL;
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    FILE *fp = fopen("input.txt", "r");
    fscanf(fp, "%d\n", &n);

    for (i = 0; i < n; i++) fscanf(fp, "%d\n", &A[i]);

    for (i = 0; i < n; i++) insert(&tree, A[i]);

    inorder(tree);
    preorder(tree);

    treefree(tree);
}