#include <stdio.h>

void matrix_multiplication(int ary[3][4][4]);


int main(void)
{
	int i, j, ary[3][4][4];

	printf("First matrix: \n");

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			scanf("%d", &ary[0][i][j]);
		}
	}

	printf("Second matrix: \n");

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			scanf("%d", &ary[1][i][j]);
		}
	} 
	
	matrix_multiplication(ary);

	printf("Result: \n");

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			printf("%8d", ary[2][i][j]);

			if(j == 3)
				printf("\n");
		}
	}

	printf("\n");

	return 0;
}

void matrix_multiplication(int ary[][4][4])
{
	int i, j, k, sum = 0;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			for(k = 0; k < 4; k++){
				sum += ary[0][i][k] * ary[1][k][j];
			}
			ary[2][i][j] = sum;
			sum = 0;
		}		
	}
}
