#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50
typedef enum {head, entry} tagfield;
typedef struct entry_node {
    int row;
    int col;
    int value;
}entry_node;
typedef struct matrix_node {
    struct matrix_node* down;
    struct matrix_node* right;
    tagfield tag;
    union {
        struct matrix_node* next;
        entry_node entry;
    }u;
}matrix_pointer;
matrix_pointer* hdnode[MAX_SIZE];

matrix_pointer* mread(FILE* fp) {
    int num_rows, num_cols, num_heads, i, j;
    int value;
    matrix_pointer *temp, *last, *node;

    fscanf(fp, "%d %d\n", &num_rows, &num_cols);
    num_heads = (num_cols > num_rows) ? num_cols : num_rows;
    node = (matrix_pointer*)malloc(sizeof(matrix_pointer));
    node->tag = entry;
    node->u.entry.row = num_rows;
    node->u.entry.col = num_cols;

    if (!num_heads) node->right = node;
    else {
        for (i = 0; i < num_heads; i++) {
            temp = (matrix_pointer*)malloc(sizeof(matrix_pointer));
            hdnode[i] = temp;
            hdnode[i]->tag = head;
            hdnode[i]->right = temp;
            hdnode[i]->u.next = temp;
        }
        for (i = 0; i < num_rows; i++) {
            last = hdnode[i];
            for (j = 0; j < num_cols; j++) {
                if (j != num_cols - 1) fscanf(fp, "%d ", &value);
                else fscanf(fp, "%d\n", &value);

                if (value) {
                    temp = (matrix_pointer*)malloc(sizeof(matrix_pointer));
                    temp->tag = entry; 
                    temp->u.entry.row = i;
                    temp->u.entry.col = j;
                    temp->u.entry.value = value;
                    last->right = temp;
                    last = temp;
                    hdnode[j]->u.next->down = temp;
                    hdnode[j]->u.next = temp;
                }
            }
            last->right = hdnode[i];
        }
        for (i = 0; i < num_cols; i++)
            hdnode[i]->u.next->down = hdnode[i];
        for (i = 0; i < num_heads - 1; i++)
            hdnode[i]->u.next = hdnode[i + 1];
        hdnode[num_heads - 1]->u.next = node;
        node->right = hdnode[0];
    }
    return node;
}

void merase(matrix_pointer **node) {
    matrix_pointer *x, *y, *head = (*node)->right;
    int i;

    for (i = 0; i < (*node)->u.entry.row; i++) {
        y = head->right;
        while (y != head) {
            x = y; y = y->right; free(x);
        }
        x = head; head = head->u.next; free(x);
    }
    y = head;
    while (y != *node) {
        x = y; y = y->u.next; free(x);
    }
    free(*node); *node = NULL;
}

void mwrite(FILE *fp, matrix_pointer* node) {
    int i, j;
    if (fp != NULL) {
        if (node != NULL) {
            matrix_pointer *temp, *head = node->right;
            fprintf(fp, "num_rows = %d, num_cols = %d\n", node->u.entry.row, node->u.entry.col);

            for (i = 0; i < node->u.entry.row; i++) {
                for (head = node->right; head != node; head = head->u.next) {
                    for (j = 0; j < node->u.entry.col; j++) {
                        for (temp = head->right; temp != head; temp = temp->right) {
                            if (temp->u.entry.row == i && temp->u.entry.col == j)
                                fprintf(fp, "%5d%5d%5d \n", temp->u.entry.row, temp->u.entry.col, temp->u.entry.value);
                        }
                    }
                }
            }
        }
        else fprintf(fp, "num_rows = 0, num_cols = 0\n");
    }
    else {
        if (node != NULL) {
            matrix_pointer *temp, *head = node->right;
            printf("num_rows = %d, num_cols = %d\n", node->u.entry.row, node->u.entry.col);

            for (i = 0; i < node->u.entry.row; i++) {
                for (head = node->right; head != node; head = head->u.next) {
                    for (j = 0; j < node->u.entry.col; j++) {
                        for (temp = head->right; temp != head; temp = temp->right) {
                            if (temp->u.entry.row == i && temp->u.entry.col == j)
                                printf("%5d%5d%5d \n", temp->u.entry.row, temp->u.entry.col, temp->u.entry.value);
                        }
                    }
                }
            }
        }
        else printf("num_rows = 0, num_cols = 0\n");
    }
}

matrix_pointer* mswitch(matrix_pointer* a, int x, int y, int z) {
    int i, j, num_heads;
    matrix_pointer *temp, *temp1, *temp2, *row1, *row2, *node, *head_node, *head1, *head2, *last, *new_node;
    node = (matrix_pointer*)malloc(sizeof(matrix_pointer));
    node->tag = entry;
    node->u.entry.row = a->u.entry.row;
    node->u.entry.col = a->u.entry.col;
    num_heads = ((a->u.entry.col) > (a->u.entry.row)) ? a->u.entry.col : a->u.entry.row;

    if (y != z) {
        if (!num_heads) node->right = node;
        else {
            head_node = a->right;
            for (i = 0; i < num_heads; i++) {
                temp = (matrix_pointer*)malloc(sizeof(matrix_pointer));
                hdnode[i] = temp;
                hdnode[i]->tag = head;
                hdnode[i]->right = temp; hdnode[i]->u.next = temp;
            }
            for (i = 0; i < (a->u.entry.row); i++) {
                temp = head_node->right;
                last = hdnode[i];
                while (temp != head_node) {
                    new_node = (matrix_pointer*)malloc(sizeof(matrix_pointer));
                    new_node->tag = temp->tag;
                    new_node->u.entry.row = temp->u.entry.row;
                    new_node->u.entry.col = temp->u.entry.col;
                    new_node->u.entry.value = temp->u.entry.value;
                    last->right = new_node;
                    last = new_node;
                    hdnode[(temp->u.entry.col)]->u.next->down = new_node;
                    hdnode[(temp->u.entry.col)]->u.next = new_node;
                    temp = temp->right;
                }
                head_node = head_node->u.next;
                last->right = hdnode[i];
            }
            for (i = 0; i < (a->u.entry.col); i++)
                hdnode[i]->u.next->down = hdnode[i];
            for (i = 0; i < num_heads - 1; i++)
                hdnode[i]->u.next = hdnode[i + 1];
            hdnode[num_heads - 1]->u.next = node;
            node->right = hdnode[0];
        }


        head_node = node->right;
        head2 = head1 = head_node;

        for (i = 0; i < y; i++)
            head1 = head1->u.next;
        for (i = 0; i < z; i++)
            head2 = head2->u.next;

        if (!x) {
            if (head1 != head1->right) {
                for (row1 = head1->right; row1 != head1; row1 = row1->right) {
                    i = row1->u.entry.col;
                    temp1 = head_node;
                    for (j = 0; j < i; j++)
                        temp1 = temp1->u.next;
                    temp = temp1;
                    temp2 = temp1->down;
                    for (; (temp2->u.entry.row != y); temp1 = temp2, temp2 = temp2->down);
                    temp1->down = temp2->down;
                    temp2->down = NULL;
                    if (row1->right == head1) last = row1;
                }
                last->right = head2;
                if (head1->right != head1) {
                    row1 = head1->right;
                    head1->right = head1;
                }
            }
            else row1 = NULL;

            if (head2 != head2->right) {
                for (row2 = head2->right; row2 != head2; row2 = row2->right) {
                    i = row2->u.entry.col;
                    temp1 = head_node;
                    for (j = 0; j < i; j++)
                        temp1 = temp1->u.next;
                    temp = temp1;
                    temp2 = temp1->down;
                    for (; (temp2->u.entry.row != z); temp1 = temp2, temp2 = temp2->down);
                    temp1->down = temp2->down;
                    temp2->down = NULL;
                    if (row2->right == head2) last = row2;
                }
                last->right = head1;
                if (head2->right != head2) {
                    row2 = head2->right;
                    head2->right = head2;
                }
            }
            else {
                row2 = NULL;
            }

            if (row1) {
                head2->right = row1;
                for (row1 = head2->right; row1 != head2; row1 = row1->right) {
                    row1->u.entry.row = z;
                    i = row1->u.entry.col;
                    temp1 = head_node;
                    for (j = 0; j < i; j++) {
                        temp1 = temp1->u.next;
                    }
                    temp = temp1;
                    if (temp->down != temp) {
                        temp2 = temp->down;
                        for (; (temp2->u.entry.row) < z && (temp2 != temp); temp1 = temp2, temp2 = temp2->down);
                        temp1->down = row1;
                        row1->down = temp2;
                    }
                    else {
                        temp->down = row1;
                        row1->down = temp;
                    }
                }
            }

            if (row2) {
                head1->right = row2;
                for (row2 = head1->right; row2 != head1; row2 = row2->right) {
                    row2->u.entry.row = y;
                    i = row2->u.entry.col;
                    temp1 = head_node;
                    for (j = 0; j < i; j++)
                        temp1 = temp1->u.next;
                    temp = temp1;
                    if (temp->down != temp) {
                        temp2 = temp->down;
                        for (; (temp2->u.entry.row) < y && (temp2 != temp); temp1 = temp2, temp2 = temp2->down);
                        temp1->down = row2;
                        row2->down = temp2;
                    }
                    else {
                        temp->down = row2;
                        row2->down = temp;
                    }
                }
            }
        }
        else {
            if (head1->down != head1) {
                for (row1 = head1->down; row1 != head1; row1 = row1->down) {
                    i = row1->u.entry.row;
                    temp1 = head_node;
                    for (j = 0; j < i; j++)
                        temp1 = temp1->u.next;
                    temp = temp1;
                    temp2 = temp1->right;
                    for (; (temp2->u.entry.col != y); temp1 = temp2, temp2 = temp2->right);
                    temp1->right = temp2->right;
                    temp2->right = NULL;
                    if (row1->down == head1) last = row1;
                }
                last->down = head2;
                if (head1->down != head1) {
                    row1 = head1->down;
                    head1->down = head1;
                }
            }
            else row1 = NULL;

            if (head2->down != head2) {
                for (row2 = head2->down; row2 != head2; row2 = row2->down) {
                    i = row2->u.entry.row;
                    temp1 = head_node;
                    for (j = 0; j < i; j++)
                        temp1 = temp1->u.next;
                    temp = temp1;
                    temp2 = temp1->right;
                    for (; (temp2->u.entry.col != z); temp1 = temp2, temp2 = temp2->right);
                    temp1->right = temp2->right;
                    temp2->right = NULL;
                    if (row2->down == head2) last = row2;
                }
                last->down = head1;
                if (head2->down != head2) {
                    row2 = head2->down;
                    head2->down = head2;
                }
            }
            else row2 = NULL;

            if (row1) {
                head2->down = row1;
                for (row1 = head2->down; row1 != head2; row1 = row1->down) {
                    row1->u.entry.col = z;
                    i = row1->u.entry.row;
                    temp1 = head_node;
                    for (j = 0; j < i; j++) {
                        temp1 = temp1->u.next;
                    }
                    temp = temp1;
                    if (temp->right != temp) {
                        temp2 = temp->right;
                        for (; (temp2->u.entry.col) < z && (temp2 != temp); temp1 = temp2, temp2 = temp2->right);
                        temp1->right = row1;
                        row1->right = temp2;
                    }
                    else {
                        temp->right = row1;
                        row1->right = temp;
                    }
                }
            }

            if (row2) {
                head1->down = row2;
                for (row2 = head1->down; row2 != head1; row2 = row2->down) {
                    row2->u.entry.col = y;
                    i = row2->u.entry.row;
                    temp1 = head_node;
                    for (j = 0; j < i; j++)
                        temp1 = temp1->u.next;
                    temp = temp1;
                    if (temp->right != temp) {
                        temp2 = temp->right;
                        for (; (temp2->u.entry.row) < y && (temp2 != temp); temp1 = temp2, temp2 = temp2->right);
                        temp1->right = row2;
                        row2->right = temp2;
                    }
                    else {
                        temp->right = row2;
                        row2->right = temp;
                    }
                }
            }
        }
        return node;
    }
    else {
        if (!num_heads) node->right = node;
        else {
            head_node = a->right;
            for (i = 0; i < num_heads; i++) {
                temp = (matrix_pointer*)malloc(sizeof(matrix_pointer));
                hdnode[i] = temp;
                hdnode[i]->tag = head;
                hdnode[i]->right = temp; hdnode[i]->u.next = temp;
            }
            for (i = 0; i < (a->u.entry.row); i++) {
                temp = head_node->right;
                last = hdnode[i];
                while (temp != head_node) {
                    new_node = (matrix_pointer*)malloc(sizeof(matrix_pointer));
                    new_node->tag = temp->tag;
                    new_node->u.entry.row = temp->u.entry.row;
                    new_node->u.entry.col = temp->u.entry.col;
                    new_node->u.entry.value = temp->u.entry.value;
                    last->right = new_node;
                    last = new_node;
                    hdnode[(temp->u.entry.col)]->u.next->down = new_node;
                    hdnode[(temp->u.entry.col)]->u.next = new_node;
                    temp = temp->right;
                }
                head_node = head_node->u.next;
                last->right = hdnode[i];
            }
            for (i = 0; i < (a->u.entry.col); i++)
                hdnode[i]->u.next->down = hdnode[i];
            for (i = 0; i < num_heads - 1; i++)
                hdnode[i]->u.next = hdnode[i + 1];
            hdnode[num_heads - 1]->u.next = node;
            node->right = hdnode[0];
        }
        return node;
    }
}

matrix_pointer* mtranspose(matrix_pointer* a) {
    int i, num_heads;
    matrix_pointer *temp, *node, *head_node, *last, *new_node;
    node = (matrix_pointer*)malloc(sizeof(matrix_pointer));
    node->tag = entry;
    node->u.entry.row = a->u.entry.col;
    node->u.entry.col = a->u.entry.row;
    num_heads = ((a->u.entry.col) > (a->u.entry.row)) ? a->u.entry.col : a->u.entry.row;

    if (!num_heads) node->right = node;
    else {
        head_node = a->right;
        for (i = 0; i < num_heads; i++) {
            temp = (matrix_pointer*)malloc(sizeof(matrix_pointer));
            hdnode[i] = temp;
            hdnode[i]->tag = head;
            hdnode[i]->down = temp; hdnode[i]->u.next = temp;
        }
        for (i = 0; i < (a->u.entry.row); i++) {
            temp = head_node->right;
            last = hdnode[i];
            while (temp != head_node) {
                new_node = (matrix_pointer*)malloc(sizeof(matrix_pointer));
                new_node->tag = temp->tag;
                new_node->u.entry.row = temp->u.entry.col;
                new_node->u.entry.col = temp->u.entry.row;
                new_node->u.entry.value = temp->u.entry.value;
                last->down = new_node;
                last = new_node;
                hdnode[(temp->u.entry.row)]->u.next->right = new_node;
                hdnode[(temp->u.entry.row)]->u.next = new_node;
                temp = temp->right;
            }
            head_node = head_node->u.next;
            last->right = hdnode[i];
        }
        for (i = 0; i < (a->u.entry.col); i++)
            hdnode[i]->u.next->right = hdnode[i];
        for (i = 0; i < num_heads - 1; i++)
            hdnode[i]->u.next = hdnode[i + 1];
        hdnode[num_heads - 1]->u.next = node;
        node->right = hdnode[0];
    }
    return node;
}

void main() {
    int x, y, z;
    scanf("%d %d %d", &x, &y, &z);
    matrix_pointer *a, *d;
    FILE *read_fp, *mswitch_fp, *mtranspose_fp, *null_fp;
    read_fp = fopen("A.txt", "r");
    mswitch_fp = fopen("switch_20171665.txt", "w");
    mtranspose_fp = fopen("transpose_20171665.txt", "w");
    null_fp = NULL;
    a = mread(read_fp);
    mwrite(null_fp, a);
    d = mswitch(a, x, y, z);
    mwrite(mswitch_fp, d);
    merase(&d);

    d = mtranspose(a);
    mwrite(mtranspose_fp, d);
    merase(&d);
    merase(&a);
    mwrite(null_fp, d);
    mwrite(null_fp, a);
    fclose(read_fp);
    fclose(mswitch_fp);
    fclose(mtranspose_fp);
}
