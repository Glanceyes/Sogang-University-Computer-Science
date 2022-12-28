#include <iostream>
#include <cstdlib>

using namespace std;

template <class T>
class Array {
protected:
    T * data;
    int len;
public:
    Array() {};
    Array(int size);
    virtual ~Array();

    int length() const;

    virtual T& operator[](int i);
    virtual T operator[](int i) const;

    void print();
};

template <class T>
Array<T>::Array(int size) {
    if (size <= 0) {
        cout << "Negative Size" << endl;
    }
    data = new T[size]; 
    len = size;
}

template <class T>
Array<T>::~Array() {
    delete[] data;
}

template <class T>
int Array<T>::length() const {
    return len;
}

template <class T>
T& Array<T>::operator[] (int i) {
    static T temp;
    if (i >= 0 && i < len) {
        return data[i];
    }
    else {
        cout << "Array Index Bound Error!" << endl;
        return temp;
    }
}

template <class T>
T Array<T>::operator[] (int i) const {
    if (i >= 0 && i < len) {
        return data[i];
    }
    else {
        cout << "Array Index Bound Error!" << endl;
        return 0;
    }
}

template <class T>
void Array<T>::print() {
    cout << '[';
    for (int i = 0; i < len; i++) {
        cout << ' ' << data[i];
    }
    cout << ']' << endl;
}