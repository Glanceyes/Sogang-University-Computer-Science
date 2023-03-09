#include <stdio.h>

double change_ary(char *, int);

int main(void)
{

	char ary[12];
	int i, n;
	double result;

	for(i = 0, n = 0; i < 12; i++){
		scanf("%c", &ary[i]);
		
		if(ary[i] == '\n')
			break;
	
		n++;

	}

	result = change_ary(ary, n);

	printf("Result: %f\n", result);

	return 0;

}


double change_ary(char ary[], int n)
{
	double result, sum = 0, digit = 1, dot = 1;

	int i, negative = 1, number;

	for(i = n; i >= 0; i--){
		number = ary[i];
		if(number >= 48 && number <= 57){
			sum += digit * ( number - 48 );
			digit *= 10;
		} 
		else if(number == 46)
			dot = digit;
		else if(number == 45)
			negative = -1;
		
	}

	result = (sum / dot) * negative;

	return result;
}
