#include <stdio.h>

int main(void)
{

	int input, i, j, k, m;

	printf("Input: ");
	scanf("%d", &input);
	
	i=0;
	j=1;
	k=1;

	while(j <= (input * 2 - 1))
	{

		while(k <= (input * 2 - 1 - j) / 2)
		{
			printf(" ");
			k++;
		}
		k=1;

		while(i < j)
		{
			printf("*");
			
			i++;	
		}
		while(k <= (input * 2 - 1 - j) / 2)
		{
			printf(" ");
		
			k++;
		}
		k=1;
		i=0;
		
		printf("\n");
		j = j + 2;


	}

	return 0;

}
