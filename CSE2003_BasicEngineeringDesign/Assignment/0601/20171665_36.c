#include <stdio.h>

int main(void)
{
	int i, j, n; 

	printf("Input: ");
	scanf("%d", &n);

	if(n % 2 == 0){
		for(i = 0; i < (n / 2); i++){
			for(j = 0; j < ((n / 2) - i); j++){
				printf("*");	
			}
			for(j = 0; j < 2 * i; j++){
				printf(" ");
			}
			for(j = 0; j < ((n / 2)- i); j++){
				printf("*");
			}
			printf("\n");
		}
		for(i = 0; i < (n / 2); i++){
			for(j = 0; j < i + 1; j++){
				printf("*");
			}
			for(j = 0; j < 2 * ( (n / 2) - (i + 1) ); j++){
				printf(" ");
			}
			for(j = 0; j < i + 1; j++){
				printf("*");
			}
			printf("\n");
		}


	}
	else if(n == 1){
		printf("*\n");
	}
	else{
		for(i = 0; i < n; i++){
			printf("*");
		}
		printf("\n");		
		for(i = 0; i < (n / 2); i++){
			for(j = 0; j < ((n / 2) - i); j++){
				printf("*");
			}
			for(j = 0; j < 2 * i + 1; j++){
				printf(" ");
			}
			for(j = 0; j < ((n / 2) - i); j++){
				printf("*");
			}
			printf("\n");		
		}	
		for(i = 0; i < (n / 2) - 1; i++){
			for(j = 0; j < i + 2; j++){
				printf("*");
			}
			for(j = 0; j < (n - 2) - (2 * (i+1)); j++){
				printf(" ");
			}
			for(j = 0; j < i + 2; j++){
				printf("*");
			}
			printf("\n");
		}
		for(i = 0; i < n; i++){
			printf("*");
		}

		printf("\n");
	}

	return 0;


}
