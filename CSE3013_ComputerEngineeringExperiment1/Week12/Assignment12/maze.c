#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NAMELEN 15

char filename[NAMELEN] = "output.maz";

int dx[4] = { -1, 1, 0, 0 };
int dy[4] = { 0, 0, 1, -1 };
int tmpmin = 0; // ���̿� �ʺ� �� ���� �� ���ؼ� �� ���� �� ����

char map[(int)1e3][(int)1e3];
char visit[(int)1e3][(int)1e3];
char **prnmap = NULL;

int **prnmapvst = NULL;
int col = 1e4, row = 1e4;
int ran[24][4];
int array[4] = { 1, 2, 4, 8 }; 
/* 1: ����, 2: ����, 4: ����, 8: ����*/

int dirtoidx[9] = { -1, 0, 1, -1, 2, -1, -1, -1, 3 }; 
// ran�� ����� �����Ϳ� ���߱� ���� �����μ� -1�� �ǹ� ����
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

void go(int x, int y)  // ��� Ž�� �˰���
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
    FILE* fp;
    int cnt = 0, ranr = 0, ranc = 0; // {1, 2, 4, 8} ���� ��ҿ� ���� ��� ������ ����
    do
    {
        for (int i = 0; i < 4; i++)
        {
            ran[cnt][i] = array[i];
        }
        cnt++;
    } while (perm());

    while (col > 1e3 || row > 1e3 || col < 1 || row < 1) // ���� ����� �ʵ���
    {
        scanf("%d\n%d", &col, &row);
    }

    prnmap = (char**)malloc(sizeof(char*) * (2 * row + 1));
    for (int i = 0; i < 2 * row + 1; i++) {
        prnmap[i] = (char*)malloc(sizeof(char) * (2 * col + 1));
    }
    
    prnmapvst = (int**)malloc(sizeof(int*) * (2 * row + 1));
    for (int i = 0; i < 2 * row + 1; i++) {
        prnmapvst[i] = (int*)malloc(sizeof(int) * (2 * col + 1));
    }

    for (int i = 0; i < 2 * row + 1; i++) {
        for (int j = 0; j < 2 * col + 1; j++) {
            prnmapvst[i][j] = 0;
        }
    }

    fp = fopen(filename, "w+");

    srand(time(0));
    go(0, 0); 

    // ù ��° �� ���
    for (int i = 0; i < col; i++) {
        prnmap[0][i * 2] = '+';
        prnmap[0][i * 2 + 1] = '-';
    }
    prnmap[0][2 * col] = '+';
    
    if (col > row) {
        tmpmin = row / 2;
    }
    else {
        tmpmin = col / 2;
    }

    // �ҿ��� �̷� �켱 �׽�Ʈ
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (j == 0)
            {
                prnmap[i * 2 + j + 1][0] = '|';
                for (int k = 0; k < col; k++)
                {
                    if (map[i][k] & 4) {
                        prnmap[i * 2 + j + 1][2 * k + 1] = ' ';
                        prnmap[i * 2 + j + 1][2 * k + 2] = ' ';

                    } // ���� ���� ��忡�� ���������� �� �� ������
                    else {
                        prnmap[i * 2 + j + 1][2 * k + 1] = ' ';
                        prnmap[i * 2 + j + 1][2 * k + 2] = '|';
                    } // ������ �� ����
                }
            }
            else
            {
                prnmap[i * 2 + j + 1][0] = '+';
                for (int k = 0; k < col; k++)
                {
                    if (map[i][k] & 2) {
                        prnmap[i * 2 + j + 1][2 * k + 1] = ' ';
                        prnmap[i * 2 + j + 1][2 * k + 2] = '+';
                    } // ���� ���� ��忡�� �Ʒ��� �� �� ������ 
                    else {
                        prnmap[i * 2 + j + 1][2 * k + 1] = '-';
                        prnmap[i * 2 + j + 1][2 * k + 2] = '+';
                    } // ������ �� ����
                }
            }
        }
    }

    cnt = 0;
    while (1) {
        ranr = rand() % (2 * row + 1);
        ranc = rand() % (2 * col + 1);
        if (ranr < 2 * row && ranr > 0 && ranc < 2 * col && ranc > 0 && (prnmap[ranr][ranc] == '-' || prnmap[ranr][ranc] == '|') && prnmapvst[ranr][ranc] == 0) {
            prnmap[ranr][ranc] = ' ';
            prnmapvst[ranr][ranc] = 1;
            cnt++;
            if (cnt == tmpmin) break;
        }
    }


    for (int i = 0; i < 2 * row + 1; i++)
    {
        for (int j = 0; j < 2 * col + 1; j++)
        {
            fprintf(fp, "%c", prnmap[i][j]);
        }
        fprintf(fp, "\n");
    }

}
