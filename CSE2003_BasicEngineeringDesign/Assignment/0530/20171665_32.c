#include <stdio.h>

int main(void)
{
	FILE* input = fopen("input32.txt", "r");
	FILE* output = fopen("output32.txt", "w");

	int num1, num2, num3, res;
	char calculator, result;


	while(1) {
		res = fscanf(input, "%d %c %d %c %d", &num1, &calculator, &num2, &result, &num3);
		if(res == EOF)
			 break;
		if(calculator == '+'){
			if( (num1 + num2) == num3)
				fprintf(output, "%d %c %d %c %d correcn\n", num1, calculator, num2, result, num3);
			else
				fprintf(output, "%d %c %d %c %d incorrect\n", num1, calculator, num2, result, num3);
		}
		else if(calculator == '-'){
			if( (num1 - num2) == num3)
				fprintf(output, "%d %c %d %c %d correct\n", num1, calculator, num2, result, num3);
			else
				fprintf(output, "%d %c %d %c %d incorrect\n", num1, calculator, num2, result, num3);
		}
		else if(calculator == '*'){
			if( (num1 * num2) == num3)
				fprintf(output, "%d %c %d %c %d correct\n", num1, calculator, num2, result, num3);
			else
				fprintf(output, "%d %c %d %c %d incorrect\n", num1, calculator, num2, result, num3);
		}
		else if(calculator == '/'){
			if( (num1 / num2) == num3)
				fprintf(output, "%d %c %d %c %d correct\n", num1, calculator, num2, result, num3);
			else
				fprintf(output, "%d %c %d %c %d incorrect\n", num1, calculator, num2, result, num3);
		}
		else if(calculator == '%'){
			if( (num1 % num2) == num3)
				fprintf(output, "%d %c %d %c %d correct\n", num1, calculator, num2, result, num3);
			else
				fprintf(output, "%d %c %d %c %d incorrect\n", num1, calculator, num2, result, num3);
		}
		
			
	}
	
	fclose(input);
	fclose(output);	

	

	return 0;
}
