#include "LinkedList.h"

template <class T>
class Stack:public LinkedList<T>{
	public :
		virtual bool Delete(T& element){
			if(this->first == NULL)
				return false;
			Node<T> *current =first;
			first =current->link;
			element =current->data;
			delete current;
			this->current_size--;
			return true;
		}
};
