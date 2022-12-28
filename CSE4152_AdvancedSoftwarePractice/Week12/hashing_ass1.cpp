#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <memory>
#include <cstdlib>
#include <random>
#include <cstdio>
#include <utility>
#include <cstring>
#include <vector>
#include <iostream>
#include <functional>

using namespace std;

std::mt19937* g_gen;

class Hashing {
public:
    Hashing() { m_ = 0; }
    Hashing(int64_t m) { m_ = m; }
    virtual void drawParams() = 0;
    virtual int64_t getHash(int64_t k) = 0;
protected:
    int64_t m_;
};

class DivHashing : public Hashing {
public:
    DivHashing(int64_t m) : Hashing() { m_ = m; printf("%d\n", m_); }
    int64_t getHash(int64_t k) {
        //TODO
        // ppt 13의 division method

        int64_t hash = k % m_;
        // return 값 작성
        return hash;
    }
    void drawParams() {

    }
private:
};

class MultiHashing : public Hashing {
public:
    MultiHashing(int64_t a, int64_t w, int64_t r) : Hashing(), a_(a), w_(w), r_(r) {
        p2w_ = 1 << w;
        m_ = p2w_ >> r;
    }
    void drawParams() {
        // MultiHashing::drawParams
        // draw parameters of hashing randomly

        std::uniform_int_distribution<int> dis;
        dis = std::uniform_int_distribution<int>(0, (p2w_ >> 2) - 1);
        int64_t num = dis(*g_gen);
        num = num + (p2w_ >> 1) + (p2w_ >> 3);
        if (!(num & 1))
            num -= 1;
        a_ = num;
        printf("p2w_, a_: %d, %d\n", p2w_, a_);
    }
    int64_t getHash(int64_t k) {
        // MultiHashing::getHash
        // Compute a hashing function.
        //TODO
        // ppt 13의 multiplication method

        int64_t hash = ((a_ * k) % p2w_) >> (w_ - r_);

        // return 값 작성
        return hash;
    }
private:
    int64_t a_, w_, r_;
    int64_t p2w_;
};

class UniversialHashing : public Hashing {
public:
    UniversialHashing(int64_t m) : Hashing(), m_(m) {
        m_ = m;
    }
    void drawParams() {

        p_ = 100001177; //manual prime number
        //p_ = 101489;

        std::uniform_int_distribution<int> dis;
        dis = std::uniform_int_distribution<int>(1, (int)(p_ >> 1) - 1);
        a_ = dis(*g_gen);
        b_ = dis(*g_gen);
        a_ += ((p_ >> 1) - 1);

    }
    int64_t getHash(int64_t k) {
        // TODO
        // // ppt 13의 Universal hashing

        int64_t hash = ((a_ * k) + b_) % p_;
        hash = hash % m_;
        // return 값 작성
        return hash;
    }

private:
    int64_t a_, b_, p_, m_;
};

int g_m = 128;
Hashing* g_mh;

void printCheck(const char* filename, int* check, int m) {
    FILE* out = fopen(filename, "w");

    for (int i = 0; i < m; i++)
        fprintf(out, "%d: %d\n", i, check[i]);

    fclose(out);
}

void computeStats(int* cnt, int m) {
    int64_t avg = 0;
    int64_t var = 0;
    int n = 0;
    int collision = 0;

    for (int i = 0; i < m; i++) {
        if (cnt[i] > 0) {

            avg += cnt[i];
            collision += (cnt[i] - 1);
            n++;
        }
    }
    avg /= n;
    for (int i = 0; i < m; i++) {
        if (cnt[i] > 0) {

            var += (cnt[i] - avg) * (cnt[i] - avg);
        }
    }
    var /= n;
    std::cout << "average: " << avg << " variance: " << var << " collision: " << collision << endl;

}

void exercise1() {

    std::random_device rd;
    std::mt19937 gen(rd());
    g_gen = &gen;

    int n = 100000;

    int* cm, * cd, * cu; //count
    cm = (int*)malloc(sizeof(int) * g_m);
    cd = (int*)malloc(sizeof(int) * g_m);
    cu = (int*)malloc(sizeof(int) * g_m);

    DivHashing dh(g_m); //128 is 1024 / 8 = 2^(10-3)
    dh.drawParams();

    MultiHashing mh(13, 10, 7);
    mh.drawParams();

    UniversialHashing uh(g_m);
    uh.drawParams();

    for (int i = 0; i < g_m; i++) {
        cm[i] = 0;
        cd[i] = 0;
        cu[i] = 0;
    }

    //collision test for random or sequential access
    std::uniform_int_distribution<int> dis;
    dis = std::uniform_int_distribution<int>(0, (int)n - 1);

    bool randomAccess = false;

    for (int i = 0; i < 1024; i++) {

        int item, key;

        if (randomAccess)
            item = dis(gen);
        else item = i;

        //please test it with and without the following sentence. 
        item = item * ((1 << 2) + (1 << 7));

        key = dh.getHash(item);
        cd[key] ++;
        key = mh.getHash(item);
        cm[key] ++;
        key = uh.getHash(item);
        cu[key] ++;
    }

    const char* filename1 = "divisionHashing.out";
    const char* filename2 = "multHashing.out";
    const char* filename3 = "universialHashing.out";

    printCheck(filename1, cd, g_m);
    printCheck(filename2, cm, g_m);
    printCheck(filename3, cu, g_m);

    cout << "Division hashing - "; computeStats(cd, g_m);
    cout << "Mult hashing - "; computeStats(cm, g_m);
    cout << "Universial hashing - "; computeStats(cu, g_m);

    g_mh = &mh;
}

int64_t computeStrHash(string const& s) {

    // ComputeStrHash ( string const& s)
    // map string to integer (only lowercase alphabets)
    // TODO 
    // refer to https://cp-algorithms.com/string/string-hashing.html#calculation-of-the-hash-of-a-string
    // 위 site의 코드 참고!

    ////////write code/////////
    
    int64_t hash_value = 0;
    int64_t p = 31;
    int64_t p_pow = 1;
    int64_t m = 1e9 + 9;
    for (char c : s) {
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }

    return hash_value;
    ///////////////////////////

}

class Node {
public:
    string key;
    int item;
    Node* nxt;
};

class LinkedList {
public:

    LinkedList() {

        //initialize head_ to NULL.
        head_ = NULL;
    }

    ~LinkedList() {

        //delete all nodes
        Node* tmp1, * tmp2;
        tmp1 = head_;
        while (tmp1 != NULL) {
            tmp2 = tmp1->nxt;
            free(tmp1);
            tmp1 = tmp2;
        }
    }

    void insert(string key, int item) {

        // insert (str:key, int: item)
        // insert item with key to the set (dictionary)
        //TODO
        //check whether there is a node that has the given key.
        Node* tmp = head_;

        
        ////////write code/////////
        /*
            1.tmp 값이 존재할 때 까지
            2.tmp의 key 값이 내가 찾는 키값이 맞을 때까지 -> 맞다면 멈춤
            3.다르다면 tmp는 다음으로 넘어가서 삽입

        */
        while (tmp != NULL) {
            if (tmp->key == key) {
                tmp->item = item;
                return;
            }
            tmp = tmp->nxt;
        }
        ///////////////////////////

        //if there is no corresponding node, create a new node and add it at head.
        if (tmp == NULL) {
            Node* tmp = new Node;
            tmp->item = item;
            tmp->key = key;
            tmp->nxt = head_;
            head_ = tmp;
        }
        //TODOEND
    }
    int find(string key) {

        // find (str:key)
        // find a item has the same key and return the item value (dictionary)
        // if don't find any matched item, return -1 
        // TODO
        Node* tmp = head_;
        

        ////////write code/////////
        /*
            링크드 리스트 탐색하여 key에 해당하는 item 찾기
            위의 insert 함수 작성한 부분에서 key 값 찾았을 때 멈추는 것 대신 item값 반환
        */
        while (tmp != NULL) { // traverse a linked list.
             // find the node having the same key.
             // return the item
            if (tmp->key == key) {
                return tmp->item;
            }
            tmp = tmp->nxt;
        }
        ///////////////////////////

        return -1;
        //TODO END
    }
    void erase(string key) {

        // erase (str:key)
        // erase the item has the same key
        // 
        //TODO 
        // find the node having the key and delete it
        Node* tmp = head_;
        Node* prev;

        if (tmp != NULL && tmp->key == key) {
            head_ = head_->nxt;
            free(tmp);
            return;
        }

        /////// Write code//////
        while (tmp != NULL) {
            if (tmp->key == key) {
                break;
            }
            prev = tmp;
            tmp = tmp->nxt;
        }
        ///////////////////////
        
        if (tmp != NULL) {
            prev->nxt = tmp->nxt;
            free(tmp);
        }
    }
private:
    Node* head_;
};

class Dictionary {
public:
    Dictionary(int m, Hashing* hash) {

        listArray_ = (LinkedList**)malloc(sizeof(LinkedList*) * m);
        hash_ = hash;
        m_ = m;
        for (int i = 0; i < m; i++)
            listArray_[i] = new LinkedList();

    }
    ~Dictionary() {
        for (int i = 0; i < m_; i++)
            delete listArray_[i];
        delete listArray_;
    }

    // find , insert, erase 부분 작성하기//
    int find(string key) {

        // find (str:key)
        // hash string to key in order to find the corresponding slot
        // then do find process in the linked list.
        // TODO

        // hash from string to int

        // 변수는 주석의 변수 사용하시면 됩니다.
        // 처음에는 주어진 key 값을 어떠한 함수에 넣으시면서 key_str을 구하시고
        // 차례대로 사용해야할 함수를 아래의 단계마다 넣으시면 됩니다.
        // 코드 실행하실 때는 변수 주석은 해제해주시면 됩니다.
        
        int64_t key_str = computeStrHash(key);
        

        // compute the slot index
        int64_t key_mem = hash_->getHash(key_str);
        

        //find the corresponding node
        return listArray_[key_mem]->find(key);
        

    }

    void insert(string key, int item) {

        // insert (str:key, integer: item)
        // hash string to key in order to find the corresponding slot
        // then do insert process in the linked list.
        
        // TODO
        // hash from string to int
        
        int64_t key_str = computeStrHash(key);
        

        // compute the slot index
        
        int64_t key_mem = hash_->getHash(key_str);
       

        // insert the item to the linked list
        
        listArray_[key_mem]->insert(key, item);
        
    }
    void erase(string key) {
        // erase (str:key)
        // hash string to key in order to find the corresponding slot
        // erase the matched item in the linked list.
        //TODO

        // hash from string to int
        
        int64_t key_str = computeStrHash(key);
        

        // compute the slot index
        
        int64_t key_mem = hash_->getHash(key_str);
        

        //delete the key from the linked list.
        
        listArray_[key_mem]->erase(key);
        
    }

private:
    Hashing* hash_;
    LinkedList** listArray_;
    int size_, m_;
};

void exercise2() {
    //Exercise 2
    Dictionary dict(g_m, g_mh);

    dict.insert("abc", 1);
    dict.insert("abd", 2);
    dict.insert("abf", 3);

    string key;
    key = "abc";    printf("key %s: %d\n", key.c_str(), dict.find(key));
    key = "abd";    printf("key %s: %d\n", key.c_str(), dict.find(key));
    key = "abe";    printf("key %s: %d\n", key.c_str(), dict.find(key));
    dict.erase("abf");
    key = "abf";    printf("key %s: %d\n", key.c_str(), dict.find(key));
}

int main() {

    exercise1();
    exercise2();
    return 0;

}