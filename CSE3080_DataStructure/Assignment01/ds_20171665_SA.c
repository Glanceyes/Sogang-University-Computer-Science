#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n, row, col, num1 = 0, num2 = -1, way = 1, number = 1;
    int **arr;

    printf("INPUT N: ");
    scanf("%d", &n);

    arr = (int**)malloc(sizeof(int*)*n);
    for (int i = 0; i < n; i++) {
        arr[i] = (int*)malloc(sizeof(int**)*n);
    }

    row = col = n;

    while (1) {
        for (int i = 0; i < col; i++, number++) {
            num2 += way;
            arr[num1][num2] = number;
        }
        row--;
        if (row < 0 || col < 0) break;
        for (int i = 0; i < row; i++, number++) {
            num1 += way;
            arr[num1][num2] = number;
        }
        col--;
        way *= -1;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%5d  ", arr[i][j]);
        }
        printf("\n");
    }

    return 0;
}