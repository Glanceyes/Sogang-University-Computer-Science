#include <stdio.h>
#include <stdlib.h>

typedef struct node{
	char value;
	struct node* next;
	struct node* prev;
} NODE;


void left(NODE** cursor){
	NODE* temp = (*cursor)->prev;
	if((*cursor)->prev != NULL){
		(*cursor) = temp;
	}
	return;
}
void right(NODE** cursor){
	NODE* temp = (*cursor)->next;
	if((*cursor)->next != NULL){
		(*cursor) = temp;
	}
	return;
}

void del(NODE** cursor){
	NODE* tempNode = (*cursor);

	if((*cursor)->prev != NULL && (*cursor)->value != '|'){
		if((*cursor)->prev != NULL){
			(*cursor)->prev->next = (*cursor)->next;
		}
		if((*cursor)->next != NULL){
			(*cursor)->next->prev = (*cursor)->prev;
		}
		(*cursor) = (*cursor)->prev;
		free(tempNode);
	}
	return;
}
void add(NODE** cursor, char* data){
	NODE* temp = (*cursor);
	int i = 0;
	while(data[i] != '\0'){
		NODE* newNode = (NODE*)malloc(sizeof(NODE));
		newNode->value = data[i];
		newNode->prev = temp;
		newNode->next = temp->next;
		temp->next = newNode;
		if(newNode->next != NULL){
			newNode->next->prev = newNode;
		}
		temp = newNode;
		(*cursor) = temp;
		i++;
	}
	return;
}
void quit(NODE* head){
	FILE* fp = NULL;
	char output;
	fp = fopen("data.txt", "w");
	
	if(head->next != NULL){
		NODE* cur = head->next;
		while(cur->next != NULL){
			output = cur->value;
			fprintf(fp, "%c", cur->value);
			cur = cur->next;
		}
		fprintf(fp, "%c", cur->value);
	}
	fclose(fp);

	return;
}

void makeList(NODE** head, char element){
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	NODE *prev = NULL, *cur = NULL;
	prev = *head;
	cur = *head;
	newNode->value = element;
	newNode->next = NULL;
	newNode->prev = NULL;

	if((*head) == NULL){
		(*head) = newNode;
		return;
	}
	
	if((*head)->next == NULL){
		(*head)->next = newNode;
		newNode->prev = (*head);
		return;
	}

	while(cur->next != NULL){
		prev = cur;
		cur = cur->next;
	}
	cur->next = newNode;
	newNode->prev = cur;
	return;
}

void findCursor(NODE** head, NODE** cursor){
	NODE* prev = NULL;
	NODE* cur = NULL;

	prev = cur = *head;

	while(cur->next != NULL){
		prev = cur;
		cur = cur->next;
	}
	*cursor = cur;
	return;
}

int main(){
	NODE* head = NULL, *cursor = NULL;
	char input, command;
	int i, flag = 0;
	char* addInput = NULL;
	addInput = (char*)malloc(sizeof(char) * 101);

	for(i = 0; i < 101; i++){
		addInput[i] = '\0';
	}

	head = (NODE*)malloc(sizeof(NODE));
	head->value = '|';
	head->next = NULL;
	head->prev = NULL;

	while(1){
		scanf("%c", &input);
		if(input == '\n') break;
		makeList(&head, input);
	}
	findCursor(&head, &cursor);
	while(1){
		scanf("%c", &command);
		switch(command){
		case 'A':
			i = 0;
			scanf(" ");
			while(1){
				scanf("%c", &input);
				if(input == '\n') break;
				addInput[i] = input;
				i++;
			}
			add(&cursor, addInput);
			for(i = 0; i < 101; i++){
				addInput[i] = '\0';
			}
			break;
		case 'L':
			left(&cursor);
			break;
		case 'R':
			right(&cursor);
			break;
		case 'D':
			del(&cursor);
			break;
		case 'Q':
			quit(head);
			flag = 1;
			break;
		default: break;
		}
		if(flag == 1) break;
	}

	return 0;
}