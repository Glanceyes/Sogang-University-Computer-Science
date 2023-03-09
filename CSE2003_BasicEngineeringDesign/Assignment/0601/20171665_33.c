#include <stdio.h>

int main(void)
{
	FILE* input = fopen("student.txt", "r");
	FILE* output = fopen("output33.txt", "w");

	int res, i, j = 0, m;

	float mid, final, sum = 0, avg = 0, average[100], total_avg = 0;

	char name[100][6], grade[100];

	j = 0;

	while(1){
		for(i = 0; i < 6; i++){
			name[j][i] = fgetc(input);
			
			if(name[j][i] == '\n' || name[j][i] == ' ') break;
			else if(name[j][i] == EOF) break;
		}

		res = fscanf(input, " %f %f\n", &mid, &final);
		
		if(res == EOF) break;

		avg = (mid + final) / 2;

		sum += avg;

		avg = 0;
		j++;
	}

	total_avg = sum / j;

	j = 0;
	
	fclose(input);

	FILE* input2 = fopen("student.txt", "r");
	
	
	while(1){	
		for(i = 0; i < 6; i++){
			name[j][i] = fgetc(input2);

			if(name[j][i] == '\n' || name[j][i] == ' ') break;
			else if(name[j][i] == EOF) break;
	
		}


		res = fscanf(input2, " %f %f\n", &mid, &final);

		if(res == EOF) break;

		average[j] = (mid + final) / 2;

		if(total_avg < average[j]){
			grade[j] = 'p';
		} 
		else if(total_avg > average[j]){
			grade[j] = 'f';
		}

		j++;

	}


	
	fprintf(output, "student		average grade\n");

	for(m = 0; m <= j - 1; m++){
		for(i = 0; i < 6; i++){
			if(name[m][i] == '\n' || name[m][i] == ' ') break;
			else fputc(name[m][i], output);
		}

		fprintf(output, "		%.2f	%c\n", average[m], grade[m]);
			
	}

	fprintf(output, "total avg : %.2f", total_avg);

	fclose(input2);
	fclose(output);
		

	return 0;

}
