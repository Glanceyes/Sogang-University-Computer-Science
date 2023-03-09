#include <stdio.h>

int reverse(int *);

int main(void)
{
	int x, result;

	printf("Input number: ");
	scanf("%d", &x);

	result = reverse(&x);

	printf("Result: %d\n", result);
}

int reverse(int *x)
{
	int i, x_paste, counter = 0, k = 1, n = 1, m = 1, digit_number, result = 0;

	x_paste = *x;

	while(x_paste > 0)
	{
		x_paste = x_paste / 10;
		counter++;
	}	

	digit_number = counter;

	while(k <= counter)
	{
		while(n < digit_number)
		{
			m = m * 10;
			n++;
		}
		
		result = result + ( (*x % 10) * m);
		
		*x = *x / 10;		

		m = 1;
		n = 1;
		k++;
		digit_number--;
	
	}

	return result;

}
