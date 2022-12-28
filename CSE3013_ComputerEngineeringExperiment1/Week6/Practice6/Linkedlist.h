#include <iostream>
template <class T>
class Node{
	public:
		T data;
		Node* link;
		Node(T element){
			data=element;
			link=NULL;
		}
};

//Linked List Class
template <class T>
class LinkedList{
	protected:
		Node<T>* first;
		int current_size;
	public:
		LinkedList(){
			first=NULL;
			current_size=0;
		};
		int GetSize(){
			return current_size;
		};//노드 개수를 return
		void Insert(T element);//맨 앞에 원소를 삽입
		virtual bool Delete(T &element);//맨 뒤의 원소를 삭제
		void Print();//리스트를 출력
};
template <class T>
void LinkedList<T>::Insert(T element){//새 노드를 맨 앞에 붙임
	Node<T> *newnode =new Node<T>(element);
	newnode->link=first;
	first=newnode;
	current_size++;
}
template <class T>
bool LinkedList<T>::Delete(T &element){//마지막 노드의 값을 리턴 하면서, 메모리에서 할당 해제(queue)
	if(first == 0)
		return false;
	Node<T> *current=first, *previous=NULL;
	while(1){
		if(current->link==0){//fine end Node
			if(previous)
				previous->link=current->link;
			else
				first=first->link;
			break;
		}
		previous=current;
		current=current->link;
	}
	element=current->data;
	delete current;
	current_size--;
	return true;
}
template <class T>
void LinkedList<T>::Print(){
	if(first == 0)
		cout<<"STack or LinkedList is Empty"<<endl;
	else{
		int counter=1;
		Node<T>*current=first;
		while(current != 0){
			cout<<"["<<counter<<"|"<<current->data<<"]";
			counter++;
			if(counter >current_size) break;
			cout<<"->";
			current=current->link;
		}
		cout<<"\n";
	}
}
