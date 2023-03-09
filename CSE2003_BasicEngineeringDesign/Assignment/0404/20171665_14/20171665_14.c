#include <stdio.h>

int checkyear(int , int *, int);

void main(void)
{
    int year, x, y, z, w, near;

    printf("Enter the year to be tested: ");
    scanf("%d", &year);

    x = year % 4;
    y = year % 100;
    z = year % 400;
    w = (year / 4) * 4;

    checkyear(year, &near, w);

    if ( ((x == 0) && (y != 0)) || (z == 0)) {
        printf("It is a leap year.\n");
    }
    else {
        printf("It is not a leap year.\n%d is a leap year.\n", near);
    }

}

int checkyear(int year, int *near, int w) {

    if (((w % 100) == 0) && ((w % 400) != 0)) {
        *near = ((year / 4) - 1) * 4;
    }
    else {
        *near = (year / 4) * 4;
    }

    return *near;

}