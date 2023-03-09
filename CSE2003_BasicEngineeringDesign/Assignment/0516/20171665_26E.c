#include <stdio.h>

void hanoi(int, char, char, char);

int main(void)
{
	int n;
	char s, a, d;

	printf("How many disks?");
	scanf("%d", &n);

	hanoi(n, 'S', 'A', 'D');
	return 0;
}

void hanoi(int n, char s, char a, char d)
{
	if(n == 1){
		printf("Move discus 1 from %c to %c\n", s, d);
		return;
	}
	else{
		hanoi(n - 1, s, d, a);
		printf("Move discus %d from %c to %c\n", n, s, d);
		hanoi(n - 1, a, s, d); 
	}

}

