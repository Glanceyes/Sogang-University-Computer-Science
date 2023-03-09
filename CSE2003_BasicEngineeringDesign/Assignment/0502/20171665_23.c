#include <stdio.h>

void func_gcd(int, int, int*);


int main(void)
{
	int a, b, gcd;

	printf("Input first number: ");
	scanf("%d", &a);

	printf("Input second number: ");
	scanf("%d", &b);
	
	func_gcd(a, b, &gcd);
	
	printf("GCD: %d\n", gcd);

}

void func_gcd(int a, int b, int* gcd)
{
	int num1, num2, temp;	

	if(a > b){
		num1 = a;
		num2 = b;
	}
	else{
		num1 = b;
		num2 = a;
	}
		
	temp = num1 % num2;
	
	if(temp > 0){	
		while(temp > 0){
			temp  = num1 % num2;
			num1 = num2;
			num2 = temp; 
		}
		*gcd = num1;
	}
	else{
		*gcd = num2;
	}

}
