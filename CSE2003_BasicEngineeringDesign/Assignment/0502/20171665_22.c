#include <stdio.h>

void swap(int *, int *);
int sum_calculator(int, int);

int main(void)
{
	int first, second, result;

	printf("Input first number: ");
	scanf("%d", &first);

	printf("Input second number: ");
	scanf("%d", &second);

	if(first < 1 || second < 1){
		printf("Input is less than 1\n");
	}
	else{		
		swap(&first, &second);
		result = sum_calculator(first, second);
	printf("Result: %d\n", result);
	}
}

void swap(int *first, int *second)
{
	int temp;

	if(*first > *second){
		temp = *first;
		*first = *second;
		*second = temp;
	}
	else{}

}

int sum_calculator(int first, int second)
{
	int i, sum = 0;

	for(i = first ; i <= second ; i++)
	{
		if( (i % 2) == 0 || (i % 3) == 0){
			sum += i;
		}
		else{}

	}

	return sum;
}
