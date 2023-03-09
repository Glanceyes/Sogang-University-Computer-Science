#include <stdio.h>
#include <time.h>

void move(int n, char from, char to) {
    printf("¿øÆÇ %d: %c->%c\n", n, from, to);
    return;
}

void hanoi(int n, char from, char by, char to) {
    if (n == 1) move(n, from, to);
    else {
        hanoi(n - 1, from, to, by);
        move(n, from, to);
        hanoi(n - 1, by, from, to);
    }
}

int main() {
    int n;
    double duration;
    clock_t start;

    printf("n=");
    scanf("%d", &n);

    start = clock();
    hanoi(n, 'A', 'C', 'B');
    duration = ((double)(clock() - start)) / CLOCKS_PER_SEC;

    printf("\nTime: %.3f ms\n", duration);

    return 0;
}