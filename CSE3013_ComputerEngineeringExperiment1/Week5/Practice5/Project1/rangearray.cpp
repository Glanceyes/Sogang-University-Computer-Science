#include <iostream>
#include <cstdlib>
#include "rangearray.h"
using namespace std;

RangeArray::RangeArray(int l, int h) :Array(h - l + 1) {
    low = l;
    high = h;
}
RangeArray::~RangeArray() {

}
int RangeArray::baseValue() {
    return low;
}
int RangeArray::endValue() {
    return high;
}
int &RangeArray::operator[](int i) {
    return Array::operator[](i - low);
}
int RangeArray::operator[](int i)const {
    return Array::operator[](i - low);
}
