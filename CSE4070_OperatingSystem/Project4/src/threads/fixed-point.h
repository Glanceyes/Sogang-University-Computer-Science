#include <stdio.h>
#include <inttypes.h>
#define FLOAT_F (1 << 14)

int int_to_float (int);
int float_to_int (int);
int float_to_int_round (int);
int float_add (int, int);
int float_add_int (int, int);
int float_subtract (int, int);
int float_subtract_int (int, int);
int float_multiply (int, int);
int float_multiply_int (int, int);
int float_divide (int, int);
int float_divide_int (int, int);