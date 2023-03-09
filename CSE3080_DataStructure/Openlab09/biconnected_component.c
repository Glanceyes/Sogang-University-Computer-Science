#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 30
#define MIN2(x, y) ((x) < (y) ? (x) : (y));

typedef struct nodePointer *nodePtr;
typedef struct nodePointer {
    nodePtr link;
    int vertex;
} nodePointer;

typedef struct stackPointer *stackPtr;
typedef struct stackPointer {
    stackPtr link;
    int current;
    int child;
} stackPointer;

stackPtr top = NULL;

short int dfn[MAX_VERTICES];
short int low[MAX_VERTICES];
nodePtr graph[MAX_VERTICES];
int num;

void push(int u, int w) {
    stackPtr temp;
    temp = (stackPtr)malloc(sizeof(stackPointer));
    temp->current = u;
    temp->child = w;
    temp->link = top;
    top = temp;
}

void pop(int* u, int* w) {
    stackPtr temp = top;
    if (!temp) printf("Stack is empty!\n");
    *u = temp->current;
    *w = temp->child;
    top = temp->link;
    free(temp);
}

void init(int n) {
    int i;
    for (i = 0; i < n; i++) {
        dfn[i] = low[i] = -1;
    }
    num = 0;
}
void bicon(int u, int v) {
    nodePtr ptr;
    int w, x, y;
    dfn[u] = low[u] = num++;
    for (ptr = graph[u]; ptr; ptr = ptr->link) {
        w = ptr->vertex;
        if (v != w && dfn[w] < dfn[u]) 
            push(u, w);
        if (dfn[w] < 0) {
            bicon(w, u);
            low[u] = MIN2(low[u], low[w]);
            if (low[w] >= dfn[u]) {
                printf("New Biconnected Component: ");
                do {
                    pop(&x, &y);
                    printf(" <%d, %d>", x, y);
                } while (!((x == u) && (y == w)));
                printf("\n");
            }
        }
        else if (w != v) low[u] = MIN2(low[u], dfn[w]);
    }
}

int main() {
    
    int n;
    int index, input;
    char buffer;
    nodePtr tail = NULL;
    FILE *fp = fopen("input.txt", "r");

    fscanf(fp, "%d\n", &n);

    for (int i = 0; i < n; i++) {
        fscanf(fp, "%d%c", &index, &buffer);
        if (buffer == 13) continue;
        tail = graph[index];
        while (EOF != fscanf(fp, "%d", &input)){
            nodePtr temp;
            temp = (nodePtr)malloc(sizeof(nodePointer));
            temp->vertex = input;
            temp->link = NULL;
            tail->link = temp;
            tail = temp;
            fscanf(fp, "%c", &buffer);
            if (buffer == 13) break;
        }
    }



    return 0;
}