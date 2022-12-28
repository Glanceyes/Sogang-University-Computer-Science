#include <iostream>
#include <queue>
#include <deque>
#include <stack>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

//그래프의 인접 행렬 표현,adj[i][j] = i와 j사이의 간선의 수
vector<vector<int> > adj;
//graph[i][j]=i로 시작해서 j로 끝나는 단어의 목록
vector<string> graph[26][26];
//indegree[i]=i로 끝나는 단어의 수
//outdegree[i]=i로 시작하는 단어의 수
vector<int> indegree, outdegree;

// dfs를 이용하여 그래프에서 circuit 정보를 획득
void getEulerCircuit(int here, vector<int>& circuit) {
    for (int there = 0; there < adj[here].size(); there++) {
        while (adj[here][there] > 0) {  // 현재 정점(here)에서 연결된 모든 간선에 대해 반복
            adj[here][there]--; // 현재 간선을 지움
            getEulerCircuit(there, circuit);
        }
    }
    circuit.push_back(here);
}

//현재 그래프가 오일러 써킷인지 오일러 트레일인지 판단하여 해당되는 그래프를 dfs 형태의 getEulerCircuit 함수를 이용하여 리턴
vector<int> getEulerTrailOrCircuit() {
    vector<int> circuit;
    // 오일러 트레일인 경우 : 시작점이 존재하므로 시작점에서 dfs 시작
    for (int i = 0; i < 26; i++)
        if (outdegree[i] - indegree[i] == 1) {  // 현재 정점이 시작점일 조건
            getEulerCircuit(i, circuit);
            return circuit;
        }
    // 오일러 서킷인 경우 : 간선에 인접한 아무 정점에서 dfs를 시작
    for (int i = 0; i < 26; i++)
        if (outdegree[i] > 0) { // 현재 정점이 간선이 있는 정점일 조건          
            getEulerCircuit(i, circuit);
            return circuit;
        }
    //모두 실패한 경우 빈 배열을 반환한다.
    return circuit;
}
//현재 그래프의 오일러 서킷/트레일 여부를 확인
bool checkEuler() {
    int plus1 = 0, minus1 = 0;  // plus1 : 그래프의 임의의 시작점, minus1 : 그래프의 임의의 끝점
    for (int i = 0; i < 26; i++) {
        int delta = outdegree[i] - indegree[i];
        if (delta < -1 || 1 < delta) return false;  // 시작점과 끝점을 제외한 정점에 들어오는 간선의 수와 나가는 간선의 수가 다르면 false 리턴
        if (delta == 1) plus1++;    // 현재 정점이 시작점의 경우
        if (delta == -1) minus1++;  // 현재 정점이 끝점의 경우
    }
    // 오일러 서킷 : 시작점과 끝점 둘 다 없어야 함
    // 오일러 트레일 : 시작점과 끝점이 각각 하나씩만 있어야함
    // 주어진 단어를 모두 이용하여 끝말잇기를 하려면 그래프가 오일러 서킷이거나 오일러 트레일이어야 함
    return (plus1 == 1 && minus1 == 1) || (plus1 == 0 && minus1 == 0);  // 시작점과 끝점이 각각 하나씩 있거나 하나도 없으면 True 아니면 False
}

void makegraph(const vector<string>& words) {
    //전역 변수 초기화
    for (int i = 0; i < 26; i++)
        for (int j = 0; j < 26; j++)
            graph[i][j].clear();
    adj = vector<vector<int> >(26, vector<int>(26, 0));     // 모든 원소가 0(int형)인 26 x 26 크기의 2차원 vector 변수 adj 정의
    indegree = outdegree = vector<int>(26, 0);

    //각 단어를 그래프에 추가한다.
    for (int i = 0; i < words.size(); i++) {    // 입력된 모든 단어에 대해 반복
        int a = words[i][0] - 'a';  // (i번째로 입력된 단어의 첫 알파벳) - 'a'    , int형인 이유는 아스키코드로 알파벳을 표현하기 때문
        int b = words[i][(int)words[i].length() - 1] - 'a';  // (i번째로 입력된 단어의 마지막 알파벳) - 'a'

        // graph, adj, outdegree, indegree 변수에 a로 시작하고 b로 끝나는 단어 정보 추가
        graph[a][b].push_back(words[i]);
        adj[a][b]++;
        outdegree[a]++;
        indegree[b]++;
    }
}
string solve(const vector<string>& words) {
    makegraph(words);   // 끝말잇기 여부를 알기 위해 알파벳간의 관계를 그래프(인접행렬 형태)로 저장하는 과정
    //차수가 맞지않으면 실패!
    if (!checkEuler()) return "IMPOSSIBLE";     // 그래프가 Euler circuit, Euler trail 둘 다 아니라면 끝말잇기 구현 불가능
    //오일러 서킷이나 경로를 찾아낸다.
    vector<int> circuit = getEulerTrailOrCircuit();
    //모든 간선을 방문하지 못했으면 실패!
    if (circuit.size() != words.size() + 1) return "IMPOSSIBLE";

    //아닌 경우 방문순서를 뒤집은 뒤 간선들을 모아 문자열로 만들어 반환한다.
    reverse(circuit.begin(), circuit.end());
    string ret;
    for (int i = 1; i < circuit.size(); i++) {
        int a = circuit[i - 1], b = circuit[i];
        if (ret.size()) ret += " ";
        ret += graph[a][b].back();
        graph[a][b].pop_back();
    }
    return ret;
}
int main(void) {
    int cases;
    cin >> cases;   // test case 개수 입력
    while (cases--) {   // 모든 test case에 대해 반복
        adj.clear();    // 인접행렬 초기화
        int n;
        cin >> n;   // 끝말잇기 여부와 순서를 알아볼 단어 개수 n 입력
        vector<string> words;
        for (int i = 0; i < n; i++) {
            string s;
            cin >> s;
            words.push_back(s); // 단어를 하나씩 입력받아 vector형 변수 words에 하나씩 추가
        }
        cout << solve(words) << endl;

    }
    return 0;
}