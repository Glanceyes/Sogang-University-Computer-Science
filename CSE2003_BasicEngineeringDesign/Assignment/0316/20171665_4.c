#include <stdio.h>

void main(void)
{
	int p;
	
	printf("Price : ");
	scanf("%d", &p);
	
	printf("------------------output------------------\n");	
	printf("Month		:	   3		 6 \n");
	printf("Payment		: %10d	%10d \n", p, (int)(p*1.05*1.05*1.05));
	printf("------------------------------------------\n");
	printf("Pay/month	: %10.2f  	%10.2f \n", p/3.0, ((int)(p*1.05*1.05*1.05))/6.0);	
}
