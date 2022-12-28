#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

const int C_MAX = 26;

// 알파벳의 각 글자에 대한 인접 행렬 표현
vector<vector<int> > adj;
vector<int> seen, order;    // seen : 정점을 방문한 적이 있는지를 저장하는 vector, order : 정렬된 알파벳을 저장하는 vector

// 주어진 단어들로부터 알파벳 간의 선후관계 그래프를 생성한다.
void makeGraph(const vector<string>& words) {
    adj = vector<vector<int> >(C_MAX, vector<int>(C_MAX, 0));
    // 모든 원소가 0(int형)인 26 x 26 크기의 2차원 vector 변수 adj 정의
    //  26인 이유 = 알파벳 개수가 26개이기 때문

    // j-1번째 단어와 j번째 단어의 사전식 순서 비교
    for (int j = 1; j < words.size(); ++j) {
        int i = j - 1, len = min(words[i].size(), words[j].size());
        for (int k = 0; k < len; ++k)
            if (words[i][k] != words[j][k]) {    // 두 단어 words[i]와 words[j]의 k번째 알파벳이 서로 다를 때
                                                                // ex) lotte와 lg는 첫 번째 알파벳이 l로 같기 때문에 그 다음 알파벳인 o와 g를 비교해야함
                int a = words[i][k] - 'a';  // 아스키코드를 이용하여 알파벳을 int형 변수로 저장    ex) 'a' = 97, 'b' = 98, 'c' = 99 .........
                int b = words[j][k] - 'a';
                adj[a][b] = 1;  // 인접 행렬 adj에 알파벳의 순서 관계 저장. ex) adj[a][b] => 알파벳 a 다음 순서로 b가 있음
                break;  // 다음 word 쌍 비교
            }
    }
}

void dfs(int here) {
    seen[here] = 1; // 현재 정점(here)를 방문하였으니 0 => 1로 state 변경
    for (int there = 0; there < adj.size(); there++)
        if (adj[here][there] & !seen[there])   // if (현재 정점에 대한 인접 정점이 있고, 해당 인접 정점을 방문한 적이 없으면)                                               // Hint : 현재 정점에 대한 인접 정점 정보는 adj를, 방문 기록은 seen vector를 이용)
            dfs(there);     // 위 조건을 만족하면 해당 인접 정점으로 재귀(recursive)
    order.push_back(here);  // 현재 정점을 order의 맨 뒤에 추가
}

//adj에 주어진 그래프를 위상정렬한 결과를 반환
vector<int> topologicalSort() {
    int m = adj.size();     // 인접행렬의 크기 = 26 (= 알파벳 개수)
    seen = vector<int>(m, 0);
    order.clear();
    for (int i = 0; i < m; i++) if (!seen[i]) dfs(i);
    reverse(order.begin(), order.end());   // vector형 변수 order를 반전(reverse)시킴
    // 그래프에 정점 관계 중 사이클이 존재하면(= Directed Acycle Graph가 아니라면) 빈 벡터 반환
        for (int i = 0; i < m; i++)
            for (int j = i + 1; j < m; j++)
                if (adj[order[j]][order[i]])
                    return vector<int>();
    //없는 경우라면 깊이 우선 탐색에서 얻은 순서(order)를 반환
    return order;
}
int main(void) {
    int cases;
    cin >> cases;   // test case 입력
    while (cases--) {   // 모든 test case에 대하여 반복
        int n;
        cin >> n;   // 입력할 단어의 개수
        vector<string> words;   // string형 원소를 가지는 vector형 변수 words 선언
        for (int i = 0; i < n; i++) {
            string s;
            cin >> s;
            words.push_back(s); // 입력된 단어를 차례대로 words에 저장
        }
        makeGraph(words);   // 입력된 단어들에 대해 인접 행렬 생성
        vector<int> solve = topologicalSort();  // 인접 행렬 words를 이용하여 암호화된 알파벳 순서를 vector형 변수 solve에 저장
        if (solve.empty())  // 입력 단어들의 알파벳 순서를 알 수 없을 경우
            cout << "INVALID HYPOTHESIS";
        else  // 입력 단어들의 알파벳 순서를 알 수 있는 경우
            for (int i = 0; i < solve.size(); i++)
                cout << char(solve[i] + 'a');   // 알파벳 순서를 출력
        cout << "\n";
    }
    return 0;
}