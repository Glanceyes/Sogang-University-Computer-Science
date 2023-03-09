#include <stdio.h>

int main(void)
{
	FILE* fp1;
	FILE* fp2;
	FILE* output;

	int total = 0, same = 0, i = 0;
	float percentage = 0;
	char filename1[100], filename2[100], c1[500], c2[500], percent = '%';

	printf("Input first file name: ");
	scanf("%s", filename1);
	printf("Input second file name: ");
	scanf("%s", filename2);
	
	fp1 = fopen(filename1, "r");
	fp2 = fopen(filename2, "r");
	output = fopen("output.txt", "w");

	i = 0;
	
	while(1){
		c1[i] = fgetc(fp1);
		while(c1[i] == '\n' || c1[i] == ' '){
			c1[i] = fgetc(fp1);
		}
		if(c1[i] == EOF) break;
		i++;
	}

	i = 0;

	while(1){
		c2[i] = fgetc(fp2);
		while(c2[i] == '\n' || c2[i] == ' '){
			c2[i] = fgetc(fp2);
		}
		if(c2[i] == EOF) break;
		i++;
	}
	
	i = 0;

	while(1){
		if(c1[i] == EOF || c2[i] == EOF) break;
		if(c1[i] == c2[i]){
			same++;
		}
		i++;
		total++;
	}

	percentage = (float)same / (float)total * 100;
	
	fprintf(output, "Total character: %d\nSame character: %d\n%.3f%c", total, same, percentage, percent);

	fclose(fp1);
	fclose(fp2);
	fclose(output);

	return 0;
}
