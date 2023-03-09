#include <stdio.h>

float approx_exfunc(float*);
double giveme_factorial(int*);

int main(void)
{
	float x, result;

	scanf("%f",&x); 

	result = approx_exfunc(&x);	
	
	printf("%f\n", result);

	return 0;
}

float approx_exfunc(float* x)
{ 
	int n = 1;
	double sum = 1, multiple = 1, difference = 1, m;

	while(difference >= 0.0000001)
	{
		m = giveme_factorial(&n);
		multiple *= *x;	
	
		sum += (multiple / m);
		n++;	
		difference = ( (multiple * *x) / giveme_factorial(&n));

		
	}

		return sum;
}

double giveme_factorial(int* n)
{
	int i;
	double sum;

	for(i = 1, sum = 1; i <= *n; i++)
	{
		sum *= i;
	}
	return sum;
} 
