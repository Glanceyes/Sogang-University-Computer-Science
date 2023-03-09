#include <stdio.h>

int main(void)
{
	int score, sum, i, class, cutline, failures;
	float avg;

	printf("Number of class: ");
	scanf("%d", &class);

	printf("Cutline: ");
	scanf("%d", &cutline);
	
	i = 0;
	failures = 0;
	sum = 0;

	while(i<class)
	{
	
		printf("Input score #%d: ", i);
		scanf("%d", &score);
		
		sum += score;
		i++;

		if(score < cutline)
			failures++;			
	}

	avg = ((float)sum / class);

	printf("Avergae score: %.2f\n", avg);
	printf("Number of failures: %d\n", failures);

	return 0;
}


