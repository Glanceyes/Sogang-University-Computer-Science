#include <stdio.h>

void Merge(int*, int*, int*);

int main(void){

	int i, arrayA[5], arrayB[5], arrayMerge[10];

	printf("Input arrayA: ");
	
	for(i = 0; i < 5; i++){
		scanf("%d", &arrayA[i]);
	}
	printf("Input arrayB: ");
	
	for(i = 0; i < 5; i++){
		scanf("%d", &arrayB[i]);
	}

	Merge(arrayA, arrayB, arrayMerge);

	for(i = 0; i < 10; i++){
		printf("%d ", arrayMerge[i]);
	}
	printf("\n");	
	return 0;

}


void Merge(int arrayA[5], int arrayB[5], int arrayMerge[10])
{
	int i = 0, j = 0, k = 0;
	
	while(i < 5 && j < 5){
		if(arrayA[i] <= arrayB[j]){
			arrayMerge[k] = arrayA[i];
			i++;
			k++;
		}
		else{
			arrayMerge[k] = arrayB[j];
			j++;
			k++;
		}

	}
	
	while(i < 5){
		arrayMerge[k] = arrayA[i];
		i++;
		k++;
	}

	while(j < 5){
		arrayMerge[k] = arrayB[j];
		j++;
		k++;
	}

}
