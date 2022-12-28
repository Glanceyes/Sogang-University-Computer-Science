#include<iostream>
#include<vector>
#include<map>
#include<queue>
#include<algorithm>
using namespace std;
// toSort[v] = v를 정렬시키는데 필요한 뒤집기 횟수
map<vector<int>, int> toSort;

// 크기가 n인 배열에 대한 모든 경우의 뒤집기 결과 및 뒤집기 비용(= 뒤집기 횟수) 계산
void precalc(int n) {
	// 모든 자연수 원소가 정렬된 n 크기의 vector형 변수 perm 정의
	vector<int> perm(n);
	for (int i = 0; i < n; i++)
		perm[i] = i;
	queue<vector<int> > q;	// bfs를 위한 queue 자료형 정의
	q.push(perm);
	toSort[perm] = 0;
	while (!q.empty()) {
		vector<int> here = q.front();
		q.pop();
		int cost = toSort[here];	// 현재 배열(here)에 대한 뒤집기 횟수
		// 모든 뒤집기 상황에 대한 이중 for문
		for (int i = 0; i < n; i++)
			for (int j = i + 2; j <= n; j++) {
				reverse(here.begin() + i, here.begin() + j); // here의 i부터 j번째까지의 원소 뒤집기(reverse)
				if (toSort.count(here) == 0) {	// 배열을 뒤집었을때 아직 계산이 안된 배열이면
					// 현재 배열 비용에 +1
					toSort[here] = cost + 1;
					q.push(here);
				}
				// 배열 원상복구
				reverse(here.begin() + i, here.begin() + j); // here의 i부터 j번째까지의 원소 뒤집기(reverse), 26번째 줄의 코드와 같음
			}
	}
}
// 입력 배열의 상대적 크기만 고려하면 되므로 정규화 시킨 후 toSort에서 뒤집기 비용 찾기
int solve(const vector<int>& perm) {
	int n = perm.size();
	vector<int> fixed(n);	// 입력 배열을 정규화한 결과를 저장할 vector형 변수 fixed

	// 입력 배열을 1~8 범위의 정수로 정규화하는 반복문 작성
	// ex) [1, 6, 2, 19, 33] => [0, 2, 1, 3, 4]
	for (int i = 0; i < n; i++) {
		int smaller = 0;
		for (int j = 0; j < n; j++)
			if (perm[j] < perm[i])
				smaller++;
		fixed[i] = smaller;
	}

	return toSort[fixed];
	 // fixed 배열에 해당하는 뒤집기 비용 리턴 (Hint : toSort와 fixed 이용)
}
int main() {
	// 1<=n<=8 모든 경우에 대한 뒤집기 결과 및 비용 계산
	for (int n = 1; n <= 8; n++){
		precalc(n);
	}

	int testcase;
	cin >> testcase;	// test case 개수 입력
	while (testcase--) {	// 모든 test case에 대하여 반복
		int n;
		cin >> n;	// 입력할 배열의 길이
		vector<int> v;
		int tmp;
		// int형 정수를 n개 만큼 입력받아 vector 변수 v에 차례대로 저장
		for (int i = 0; i < n; i++) {
			cin >> tmp;
			v.push_back(tmp);
		}
		cout << solve(v) << endl;
	}
}