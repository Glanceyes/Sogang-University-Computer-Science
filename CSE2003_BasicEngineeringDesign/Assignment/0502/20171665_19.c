#include <stdio.h>

void swap(int *, int *, int *);
void absValue(int *, int *, int *);

int main(void)
{
	int a, b, c;

	printf("Input: ");
	scanf("%d %d %d", &a, &b, &c);

	absValue(&a, &b, &c);
	swap(&a, &b, &c);
	
	printf("Result: %d %d %d\n", a, b, c);


}

void absValue(int *a, int *b, int *c){

	if(*a < 0){
		*a = *a * (-1);
	}
	else{}

	if(*b < 0){
		*b = *b * (-1);
	}
	else{}

	if(*c < 0){
		*c = *c * (-1);
	}
	else{}

}

void swap(int *a, int *b, int *c){
	int temp;

	if(*a < *b){
		temp = *a;
		*a = *b;
		*b = temp;
	}
	else{}

	if(*a < *c){
		temp = *a;
		*a = *c;
		*c = temp;
	}
	else{}

	if(*b < *c){
		temp = *b;
		*b = *c;
		*c = temp;
	}
	else{}

}
