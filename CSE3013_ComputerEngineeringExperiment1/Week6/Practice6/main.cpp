#include "Stack.h"
#include <iostream>
#include <stdio.h>
using namespace std;

void prnMenu(){
cout<<"*******************************************"<<endl;
cout<<"* 1. ���� 2. ���� 3. ��� 4. ���� *"<<endl;
cout<<"*******************************************"<<endl;
cout<<endl;
cout<<"���Ͻô� �޴��� ����ּ���: ";
}
int main(){
// ���� �� ���� ����Ʈ �׽�Ʈ�� �ڵ�
 int mode, selectNumber,tmpItem;
		LinkedList<int> *p;
		bool flag = false;

		cout<<"�ڷᱸ�� ����(1: Stack, Other: Linked List): ";
		cin>>mode;
						    
		if(mode == 1){
			p = new Stack<int>();
		}// ������ �����ϴ� ����
		else{
			p = new LinkedList<int>();
		}

		do{
			prnMenu();
			cin>>selectNumber;
			
			switch(selectNumber){ 
				case 1:
					cout<<"���Ͻô� ���� �Է����ּ���: ";
					cin>>tmpItem;    p->Insert(tmpItem);
					cout<<tmpItem<<"�� ���ԵǾ����ϴ�."<<endl;
					break;
					
				case 2:
					if(p->Delete(tmpItem)==true)
						cout<<tmpItem<<"�� �����Ǿ����ϴ�."<<endl;
						
					else cout<<"����ֽ��ϴ�. ���� ����"<<endl;
					break;
					
				case 3:
					cout<<"ũ��: "<<p->GetSize()<<endl;
					p->Print();
					break;
					
				case 4:
					flag = true;     break;	
				default:
					cout<<"�߸� �Է��ϼ̽��ϴ�."<<endl;
					break;
			
			}
			
			if(flag) break;
			
		} while(1);
		
		return 0;
}