#include <stdio.h>

int main(void)
{
	int i, j, num[20][20], n = 0;

	printf("input size of triangle : ");
	scanf("%d", &n);

	
	for(j = 0; j < n; j++){
	
		for(i = 0; i <= j; i++){
		
			if(i == 0 || i == j){
				num[j][i] = 1;
			}
			else{
				num[j][i] = num[j-1][i] + num[j-1][i-1];
			}
				
		}
	}

	for(j = 0; j < n; j++){

		for(i = 0; i <= j; i++){
			
			printf("%-3d ", num[j][i]);

			if(i == j){
				printf("\n");	
			}
		
		}

	}

	return 0;

}

