#include <stdio.h>

void main(void)
{
    int a, b;

    printf("a = ");
    scanf("%d", &a);

    printf("b = ");
    scanf("%d", &b);

    printf("a/b is %d ", a / b);
    printf("with a remainder of %d\n", a % b);
}