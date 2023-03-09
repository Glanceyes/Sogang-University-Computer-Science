#include <stdio.h>

float addition(float, float);
float substraction(float, float);
float multiplication(float, float);
float division(float, float);

int main(void)
{
	char operator;
	float operand1, operand2, result;

	printf("Operator (+, -, *, /)\n");
	printf("Please enter the formula. (ex: 12.3 + 5.5)\n");
	scanf("%f %c %f", &operand1, &operator, &operand2);

	switch((int)operator){
		case 43:
			result = addition(operand1, operand2);	
			printf("%.2f + %.2f = %.2f\n", operand1, operand2, result); break;
		case 45:
			result = substraction(operand1, operand2);
			printf("%.2f - %.2f = %.2f\n", operand1, operand2, result); break;
		case 42:
			result = multiplication(operand1, operand2);
			printf("%.2f * %.2f = %.2f\n", operand1, operand2, result); break;
		case 47:
			if(operand2 != 0){
			result = division(operand1, operand2);
			printf("%.2f / %.2f = %.2f\n", operand1, operand2, result);
			}
			else{
			printf("Division by zero\n");
			}
			break;
		default:
			printf("Invalid operator : %c\n", operator);
	}	

	return 0;
	
}

float addition(float operand1, float operand2)
{
	float result;

	result = operand1 + operand2;

	return result;
}

float substraction(float operand1, float operand2)
{
	float result;

	result = operand1 - operand2;

	return result;
}

float multiplication(float operand1, float operand2)
{
	float result;

	result = operand1 * operand2;

	return result;
}

float division(float operand1, float operand2)
{
	float result;

	result = operand1 / operand2;

	return result;
}
