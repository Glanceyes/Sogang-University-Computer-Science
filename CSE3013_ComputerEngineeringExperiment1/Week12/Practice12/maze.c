#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NAMELEN 15

char filename[NAMELEN] = "output.maz";

int dx[4] = { -1, 1, 0, 0 };
int dy[4] = { 0, 0, 1, -1 };

char map[(int)1e3][(int)1e3];
char visit[(int)1e3][(int)1e3];
int col = 1e4, row = 1e4;
int ran[24][4];
int array[4] = { 1, 2, 4, 8 }; 

int dirtoidx[9] = { -1, 0, 1, -1, 2, -1, -1, -1, 3 }; 
int opptodir[9] = { -1, 2, 1, -1, 8, -1, -1, -1, 4 }; 

int perm() 
{
    int i = 3, j = 3;
    while (i > 0 && (array[i - 1] >= array[i])) i--;
    if (i == 0) return 0;
    while (array[i - 1] > array[j]) j--;

    int tmp = array[i - 1];
    array[i - 1] = array[j];
    array[j] = tmp;

    j = 3;
    while (i < j)
    {
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
        i++; j--;
    }
    return 1;
}

void go(int x, int y)  // 재귀 탐색 알고리즘
{
    visit[x][y] = 1;
    int r = rand() % 24; 

    for (int i = 0; i < 4; i++)
    {
        int dir = ran[r][i];
        int idx = dirtoidx[dir];

        int nx = x + dx[idx], ny = y + dy[idx];

        if (nx >= row || ny >= col || nx < 0 || ny < 0) continue;
        if (visit[nx][ny]) continue; 
        map[x][y] |= dir;
        map[nx][ny] |= opptodir[dir];
        go(nx, ny);
    }
}

int main()
{
    int cnt = 0; 
    do
    {
        for (int i = 0; i < 4; i++)
        {
            ran[cnt][i] = array[i];
        }
        cnt++;
    } while (perm());

    while (col > 1e3 || row > 1e3 || col < 1 || row < 1)
    {
        scanf("%d\n%d", &col, &row);
    }
    freopen(filename, "w", stdout);

    srand(time(0));
    go(0, 0); 

    for (int i = 0; i < col; i++) printf("+-");
    printf("+");
    printf("\n");

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (j == 0)
            {
                printf("|");

                for (int k = 0; k < col; k++)
                {
                    if (map[i][k] & 4) printf("  "); //만약 현재 노드에서 오른쪽으로 가는게 있으면
                    else printf(" |");
                }
            }
            else
            {
                printf("+");
                for (int k = 0; k < col; k++)
                {
                    if (map[i][k] & 2) printf(" +");
                    else printf("-+"); // 만약 현재 노드에서 아래로 가는게 없으면
                }
            }
            printf("\n");
        }
    }
}
