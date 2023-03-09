#include <stdio.h>

void main(void)
{
    float num;

    printf("Input: ");
    scanf("%f", &num);

    printf("Use \">\": %d\n", ( ((int)(num * 10)) % 10 ) > 4);
    printf("Use \">=\": %d\n", ( ((int)(num * 10)) % 10 ) >= 5);
    printf("Use \"==\": %d\n", ( (int)((num + 0.5) * 10) ) / 10  == (int)num + 1);
    printf("Use \"!=\": %d\n", ((int)((num + 0.5) * 10)) / 10 != (int)num );

}