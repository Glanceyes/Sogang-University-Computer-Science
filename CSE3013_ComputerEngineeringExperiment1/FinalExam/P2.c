#include <stdio.h>
#include <stdlib.h>

void print(char *plain_text, char *crypt_text){
	int i;

	for(i = 0; i < 26; i++){
		printf("%c", plain_text[i]);
	}
	printf("\n");

	for(i = 0; i < 26; i++){
		printf("%c", crypt_text[i]);
	}
	printf("\n");

}

void push(char *crypt_text){
	char tempChar;
	int i = 0;
	tempChar = crypt_text[25];
	for(i = 25; i > 0; i--){
		crypt_text[i] = crypt_text[i - 1];
	}
	crypt_text[0] = tempChar;
}

void pop(char *crypt_text) {
	char tempChar;
	int i = 0;
	tempChar = crypt_text[0];
	for(i = 0; i < 25; i++){
		crypt_text[i] = crypt_text[i + 1];
	}
	crypt_text[25] = tempChar;
}

char* encrypt(char *plain_text, char *crypt_text, char *input_text) {
	int i = 0, j, flag;
	char* encrypt_text = NULL;
	encrypt_text = (char*)malloc(sizeof(char) * 101);

	for(j = 0; j < 101; j++){
		encrypt_text[j] = '\0';
	}

	while(input_text[i] != '\0'){
		flag = 0;
		for(j = 0; j < 26; j++){
			if(input_text[i] == plain_text[j]) {
				encrypt_text[i] = crypt_text[j];
				flag = 1;
				break;
			}
		}
		if (flag == 0) encrypt_text[i] = input_text[i];
		i++;
	}

	return encrypt_text;
}

char* decrypt(char *plain_text, char *crypt_text, char *input_text) {
	int i = 0, j, flag;
	char* decrypt_text = NULL;
	decrypt_text = (char*)malloc(sizeof(char) * 101);

	for(j = 0; j < 101; j++){
		decrypt_text[j] = '\0';
	}

	while(input_text[i] != '\0'){
		flag = 0;
		for(j = 0; j < 26; j++){
			if(input_text[i] == crypt_text[j]) {
				decrypt_text[i] = plain_text[j];
				flag = 1;
				break;
			}
		}
		if (flag == 0) decrypt_text[i] = input_text[i];
		i++;
	}
	return decrypt_text;
}

int main(){
	char plain_text[26], crypt_text[26];
	char input_text[101] = {'\0'};
	char *encrypt_text = NULL;
	char *decrypt_text = NULL;
	char tempChar;
	int i, j, command;
	
	encrypt_text = (char*)malloc(sizeof(char) * 101);
	decrypt_text = (char*)malloc(sizeof(char) * 101);

	i = 0;
	while(1){
		scanf("%c", &tempChar);
		if(tempChar == '\n') break;
		input_text[i] = tempChar;
		i++;
	}

	for(i = 0; i < 26; i++){
		crypt_text[i] = 65 + i;
		plain_text[i] = 65 + i;
	}

	while(1){
		scanf("%d", &command);
		switch(command){
		case 0: 
			print(plain_text, crypt_text);
			break;
		case 1: 
			push(crypt_text); break;
		case 2: 
			pop(crypt_text); break;
		case 3: 
			encrypt_text = encrypt(plain_text, crypt_text, input_text);
			i = 0;
			while(encrypt_text[i] != '\0'){
				printf("%c", encrypt_text[i]);
				i++;
			}
			printf("\n");
			break;
		case 4: 
			decrypt_text = decrypt(plain_text, crypt_text, input_text); 
			i = 0;
			while(decrypt_text[i] != '\0'){
				printf("%c", decrypt_text[i]);
				i++;
			}
			printf("\n");
			break;
		}
	}

	free(encrypt_text);
	free(decrypt_text);

	return 0;
}