#include <stdio.h>

float Column_Average(int*, int);

int main(void)
{
	int n, i, j, n_matrix[100][100], result;

	scanf("%d", &n);

	for(i = 0; i < n; i++){
		for(j = 0; j < n; j++){
			scanf("%d", &n_matrix[i][j]);
		}
	}
	
	for(i = 0; i < n; i++){
		printf("%.2f ", Column_Average(n_matrix[i], n));
	}

	printf("\n");

	return 0;
}

float Column_Average(int n_matrix[], int n)
{
	float sum = 0, average;
	int col;
	
	for(col = 0; col < n; col++){
		sum += n_matrix[col];
	}

	average = sum / n;

	return average;
}


