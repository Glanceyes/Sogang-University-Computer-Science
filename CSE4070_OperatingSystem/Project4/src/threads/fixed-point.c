#include "threads/fixed-point.h"

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

int int_to_float (int n){
    return n * FLOAT_F;
}

int float_to_int (int x){
    return x / FLOAT_F;
}

int float_to_int_round (int x){
    if (x >= 0)
        return (x + FLOAT_F / 2) / FLOAT_F;
    else
        return (x - FLOAT_F / 2) / FLOAT_F;
}

/* FLOAT + FLOAT */
int float_add (int x, int y){
    return x + y;
}

/* FLOAT + INT */
int float_add_int (int x, int n){
    return x + n * FLOAT_F;
}

/* FLOAT - FLOAT */
int float_subtract (int x, int y){
    return x - y;
}

/* FLOAT - INT */
int float_subtract_int (int x, int n){
    return x - n * FLOAT_F;
}

/* FLOAT * FLOAT */
int float_multiply (int x, int y){
    return ((int64_t) x) * y / FLOAT_F;
}

/* FLOAT * INT */
int float_multiply_int (int x, int n){
    return x * n;
}

/* FLOAT / FLOAT */
int float_divide (int x, int y){
    return ((int64_t) x) * FLOAT_F / y;
}

/* FLOAT / INT */
int float_divide_int (int x, int n){
    return x / n;
}