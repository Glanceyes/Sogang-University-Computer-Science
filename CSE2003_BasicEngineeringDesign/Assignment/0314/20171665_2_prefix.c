#include <stdio.h>

void main(void)
{
    int a;

    printf("a = ");
    scanf("%d", &a);

    printf("a = %d\n", ++a);

    printf("a = %d\n", ++a);
}
