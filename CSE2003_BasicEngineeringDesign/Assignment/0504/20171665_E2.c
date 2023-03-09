#include <stdio.h>

int Fibonacci(int *);

int main(void)
{
	int n, result;

	printf("Input number N: ");
	scanf("%d", &n);

	result = Fibonacci(&n);

	printf("Fibonacci Number: %d\n", result);
}

int Fibonacci(int *n)
{
	int i=0, j=1, k=1, m=1, result;

	while(m < *n){

	k = i + j;
	
	i = j;
	j = k;
	
	m++;

	}
	
	result = k;
	return result;
}
