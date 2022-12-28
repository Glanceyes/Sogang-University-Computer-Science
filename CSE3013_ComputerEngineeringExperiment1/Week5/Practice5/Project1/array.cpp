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
int &Array::operator [](int i) { // 연산자만을 이용해서도 함수가 호출되도록 하겠다는 의미이다.
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
