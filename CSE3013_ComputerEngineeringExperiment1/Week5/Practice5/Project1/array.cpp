#include <cstdlib>
#include <iostream>
#include "Array.h"

using namespace std;

Array::Array(int size) {
    if (size <= 0) {
        cout << "Array bound error!" << endl;
        exit(-1);
    }
    else {
        data = new int[size];
        len = size;
    }
}
Array::~Array() {
    delete[] data;
}
int Array::length() const {
    return len;
}
int &Array::operator [](int i) { // �����ڸ��� �̿��ؼ��� �Լ��� ȣ��ǵ��� �ϰڴٴ� �ǹ��̴�.
    static int tmp;
    if (i >= 0 && i<len) {
        return data[i];
    }
    else {
        cout << "Array bound error!" << endl;
        return tmp;
    }
}
int Array::operator [](int i)const {
    if (i >= 0 && i<len) {
        return i;
    }
    else {
        cout << "Array bound error!" << endl;
        return 0;

    }
}
void Array::print() {
    int i;
    cout << "[ ";
    for (i = 0; i<len; i++) {
        cout << data[i] << " ";
    }
    cout << "]\n" << endl;
}
