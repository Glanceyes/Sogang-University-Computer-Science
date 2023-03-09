#include <stdio.h>

int palindrome (int, int);


int main(void)
{
	int num, digit = 1;

	printf("Input: ");
	scanf("%d", &num);

	if(num < 0){
		printf("Input number cannot be negative.\n");
	}
	else{
		if(palindrome (num, digit) == 1)
			printf("%d is a palindrome number.\n", num);
		else
			printf("%d is not a palindrome number.\n", num);
	}
	return 0;

}

int palindrome (int num, int digit)
{
	int counter = 0, num1, num2, num3 = 1, i = 1;

	num1 = num;
	num2 = num;

	while(num1 != 0)
	{
		num1 = num1 / 10;
		counter++;	
	}
	
	for(i = 1 ; i < counter; i++)
	{
		num3 = num3 * 10;

	}

	if( ( ( num2 / (num3 / digit) ) % 10 ) ==  ( ( num2 % (digit * 10) ) / digit) ){
		if( ( (num3 / digit) == digit ) || ( (num3 / digit) == digit * 10) )
			return 1;
		else
			return palindrome(num, digit * 10);
	}
	else
		return 0;


}
