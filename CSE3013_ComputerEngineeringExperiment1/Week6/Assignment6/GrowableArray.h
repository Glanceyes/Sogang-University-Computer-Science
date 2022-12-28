#include "Array.h"

using namespace std;

template <class T>
class GrowableArray : public Array<T> {
public:
    GrowableArray(int size);
    virtual T& operator[](int i);
};

template <class T>
GrowableArray<T>::GrowableArray(int size) : Array<T>(size) {}

template <class T>
T& GrowableArray<T>::operator[](int i){
    if (i >= 0 && i < this->len) {
        return this->data[i];
    }
    else {
        int big_len = this->len;
        while (big_len <= i)
            big_len *= 2; // 인덱스가 두 배를 넘어서면 더 늘려야 될 것으로 판단했습니다.
        T* new_data = new T[big_len];
        for (int i = 0; i < this->len; i++)
            new_data[i] = this->data[i];
        for (int i = this->len; i < big_len; i++)
            new_data[i] = 0;
        T *old_data = this->data;
        this->data = new_data;
        this->len = big_len;
        delete[] old_data;
        return this->data[i];
    }
}