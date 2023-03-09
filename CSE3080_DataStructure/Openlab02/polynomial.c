#include <stdio.h>
#include <stdlib.h>
#define MAX_TERMS 100

typedef struct {
    float coef;
    int expon;
} polynomial;

int compare(int x, int y) {
    if (x < y) return -1;
    else if (x == y) return 0;
    else return 1;
}

void attach(float coefficient, int exponential, int *avail, polynomial terms[]) {
    if (*avail >= MAX_TERMS) {
        fprintf(stderr, "Too many terms in the polynomial\n");
        exit(EXIT_FAILURE);
    }
    terms[*avail].coef = coefficient;
    terms[(*avail)++].expon = exponential;
}

void readPoly(polynomial terms[], int *startA, int *finishA, int *startB, int *finishB) {

    FILE *fileA, *fileB;

    fileA = fopen("A.txt", "r");

    if (fileA == NULL) {
        printf("File Open Error.\n");
        return;
    }

    while (EOF != fscanf(fileA, "%f\t%d\n", &terms[(*finishA)].coef, &terms[(*finishA)].expon)) {
        (*finishA)++;
    };

    *finishB = *startB = *finishA;
    (*finishA)--;

    fileB = fopen("B.txt", "r");

    if (fileB == NULL) {
        printf("File Open Error.\n");
        return;
    }

    while (EOF != fscanf(fileB, "%f\t%d\n", &terms[(*finishB)].coef, &terms[(*finishB)].expon)) {
        (*finishB)++;
    };

    fclose(fileA);
    fclose(fileB);
}

void printPoly(polynomial terms[], int startC, int finishC) {

    while (1) {
        printf("(%.f, %d)", terms[startC].coef, terms[startC].expon);
        startC++;
        if (startC > finishC) break;
        printf(",   ");
    }
    printf("\n");
}

void padd(polynomial terms[], int startA, int finishA, int startB, int finishB, int *startC, int *finishC, int avail) {

    float coefficient;

    while (startA <= finishA && startB <= finishB) {
        switch (compare(terms[startA].expon, terms[startB].expon)) {
        case -1:
            attach(terms[startB].coef, terms[startB].expon, &avail, terms);
            startB++;
            break;
        case 0:
            coefficient = terms[startA].coef + terms[startB].coef;
            if (coefficient)
                attach(coefficient, terms[startA].expon, &avail, terms);
            startA++;
            startB++;
            break;
        case 1:
            attach(terms[startA].coef, terms[startA].expon, &avail, terms);
            startA++;
        }
    }
    for (; startA <= finishA; startA++)
        attach(terms[startA].coef, terms[startA].expon, &avail, terms);
    for (; startB <= finishB; startB++)
        attach(terms[startB].coef, terms[startB].expon, &avail, terms);

    *finishC = avail - 1;
}

int main(void) {

    polynomial terms[MAX_TERMS];

    int startA, finishA, startB, finishB, startC, finishC, avail;

    finishB = startB = startA = finishA = 0;

    readPoly(terms, &startA, &finishA, &startB, &finishB);

    avail = finishC = startC = finishB;
    finishB--;

    padd(terms, startA, finishA, startB, finishB, &startC, &finishC, avail);

    printPoly(terms, startC, finishC);

    return 0;
}