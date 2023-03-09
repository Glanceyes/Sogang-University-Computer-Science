#include <stdio.h>

float xequation(int, int, int, int);
float yequation(int, int, int, int);

void main (void)
{
	int a;
	int b;
	int c;
	int d;
	float x1;
	float y1;

	printf("lst equation: y = ax - b, input 'a' and 'b'\n");

	printf("a = ");
	scanf("%d", &a);

	printf("b = ");
	scanf("%d", &b);

	printf("2nd equation: y = cx - d, input 'c' and 'd'\n");

	printf("c = ");
	scanf("%d", &c);
	
	printf("d = ");
	scanf("%d", &d);

	x1 = xequation(a, b, c, d);
	y1 = yequation(a, b, c, d);

	printf("result: \nx is %f\ny is %f\n", x1, y1);
}

float xequation(int a1, int b1, int c1, int d1)
{
	float x;

	x = ((float)(b1 - d1) / (float)(a1 - c1));

	return x;
}

float yequation(int a2, int b2, int c2, int d2)
{
	float x;
	float y;

	x = ((float)(b2 - d2) / (float)(a2 - c2));
	y = ((float)(a2 * x) - (float)b2);
	
	return y;
}
