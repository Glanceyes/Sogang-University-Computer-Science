#ifndef __ARRAY__
#define __ARRAY__

class Array {
    protected:
        int *data;
        int len;
    public:
        Array(int size);
        ~Array();

        int length() const;

        int& operator[](int i);
        int operator[](int i)const; 
        // 멤버 함수 뒤에 const를 붙이는 것은 멤버 변수가 갖는 값을 가져다 쓸 수는 있어도 변경할 수는 없음을 의미한다.
        
        void print();
};


#endif