#define _CRT_SECURE_NO_WARNINGS 
#include <cstdio>
#include <memory>
#include <queue>
#include <utility>
#include <cmath>

// SegmentTree1D 구현하세요. 직접 스크래치로 전부 구현하셔도 무관합니다.
// 원소 합을 return 해주는 sum 함수와 원소 수정에 대한 modify 함수 구현

class Node {
public:
    Node(int left, int right) {

        this->left = left;
        this->right = right;
        mid = (left + right) >> 1;
        sum = 0;
        leftNode = NULL;
        rightNode = NULL;

    }
    int left, right, mid, sum;
    Node* leftNode, * rightNode;
};
typedef std::pair<int, int> pairi2; //int 형 두 개체를 단일 개체로 처리 목적

class SegmentTree1D {

public:
    SegmentTree1D(int n) {
        //n개 사이즈 만큼 할당
        this->n = n;
        m = 1;
        int temp = n;
        while (temp) {
            m <<= 1;
            temp >>= 1;
        }

        array = new int[n];
        cum_array = new int[n];
        root = new Node(0, n - 1);

    }

    void makeSegmentTree(Node* current_node) {
        if (current_node->left == current_node->right) {
            current_node->sum = array[current_node->left];
            return;
        }
        current_node->leftNode = new Node(current_node->left, current_node->mid);
        current_node->rightNode = new Node(current_node->mid + 1, current_node->right);
        makeSegmentTree(current_node->leftNode);
        makeSegmentTree(current_node->rightNode);
        current_node->sum = current_node->leftNode->sum + current_node->rightNode->sum;
    }

    // 초기화
    void initialize() {
        int sum = 0;
        for (int i = 0;i < n;i++) {
            sum += array[i];
            cum_array[i] = sum;
        }
        makeSegmentTree(root);
    }

    int sum(Node* current_node, int x, int y) {
    /*
        x에서 y 까지의 합
    */
        if (current_node->left > y || current_node->right < x) {
            return 0;
        }
        
        if (current_node->left >= x && current_node->right <= y) {
            return current_node->sum;
        }
        else {
            return sum(current_node->leftNode, x, y) + sum(current_node->rightNode, x, y);
        }
    }

    void modify(Node* current_node, int idx, int num) {
    /*
        idx에 위치한 원소를 num으로 교체
    */
        // with segment tree
        if (idx < current_node->left || idx > current_node->right) {
            return;
        }
        if (current_node->left == current_node->right) {
            current_node->sum = num;
            return;
        }

        modify(current_node->leftNode, idx, num);
        modify(current_node->rightNode, idx, num);
        current_node->sum = current_node->leftNode->sum + current_node->rightNode->sum;
    }

    int n;
    int m;
    int* array;
    int* cum_array;
    Node* root;
};

void swap(int& a, int& b) {
    int temp = a; a = b; b = temp;
}

int main() {

    int n, m;
    FILE* in = fopen("input_assignment1.txt.txt", "r");

    fscanf(in, "%d", &n);
    SegmentTree1D A(n);

    int temp;
    for (int i = 0;i < n;i++) {
        fscanf(in, "%d", &temp);
        printf("%d ", temp);
        A.array[i] = temp;
    }
    printf("\n");

    A.initialize();

    fscanf(in, "%d", &m);
    for (int i = 0;i < m;i++) {

        fscanf(in, "%d\n", &temp);

        if (temp == 0) {
            int st, ed;
            fscanf(in, "%d %d", &st, &ed);

            if (st > ed) {
                swap(st, ed);
            }
            
            printf("sum (%d,%d): %d\n", st, ed, A.sum(A.root, st, ed));

        }
        else {
            int idx, num;
            fscanf(in, "%d %d", &idx, &num);
            printf("change %dth elem with %d\n", idx + 1, num);
            A.modify(A.root, idx, num);
        }
    }

    return 0;

}