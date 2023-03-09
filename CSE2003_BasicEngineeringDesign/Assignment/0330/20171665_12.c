#include <stdio.h>

int x;

int Fibonacci (int *);

void main (void)
{
	int x;
	
	x = 1;	
	Fibonacci (&x);

	printf("Fibo(2): %d\n", x);

	Fibonacci (&x);

	printf("Fibo(3): %d\n", x);

	Fibonacci (&x);

	printf("Fibo(4): %d\n", x);

	Fibonacci (&x);

	printf("Fibo(5): %d\n", x);

	Fibonacci (&x);

	printf("Fibo(6): %d\n", x);

	Fibonacci (&x);

	printf("Fibo(7): %d\n", x);

	Fibonacci (&x);

	printf("Fibo(8): %d\n", x);

	Fibonacci (&x);

	printf("Fibo(9): %d\n", x);

}

int Fibonacci (int *y)
{
	int result;
	result = *y + x;
	x = *y;
	*y = result;

	return *y; 
} 
