#include <stdio.h>

void main(void)
{
	int a;
	int b;
	int c;

	printf("Birthday : ");
	scanf("%d", &a);	
	printf("Your birthday is %4d / %d / %2d \n", a/10000, (a%10000)/100, a%100);

	printf("Year : ");
	scanf("%d", &b);
	c = (b-(a/10000))+1;
	printf("In %d, your age is %d \n", b, c);
}


