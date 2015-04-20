#include <cstdio>
#include <algorithm>
#include <cmath>
using namespace std;

const long long MaxL = 1000000000000000000LL;
const int MaxI = 100000000;
const int MaxA = 1000 + 10;
const int MaxB = 100 + 10;
int xa[MaxA], ya[MaxA];
int xb[MaxB], yb[MaxB];
int a, b;

long long calc(int x, int y)
{
	long long ans = 0;
	for (int i = 0; i < a; ++i) {
		ans += (long long)(x - xa[i]) * (x - xa[i]);
		ans += (long long)(y - ya[i]) * (y - ya[i]);
	}
	int ansb = MaxI;
	for (int i = 0; i < b; ++i)
		ansb = min(ansb, abs(x - xb[i]) + abs(y - yb[i]));
	ans += ansb;
	return ans;
}

void Work(int Test)
{
	int n, m;
	scanf("%d%d%d%d", &n, &m, &a, &b);
	long long sumx = 0, sumy = 0;
	for (int i = 0; i < a; ++i) {
		scanf("%d%d", &xa[i], &ya[i]);
		sumx += xa[i];
		sumy += ya[i];
	}
	for (int i = 0; i < b; ++i) {
		scanf("%d%d", &xb[i], &yb[i]);
	}
	sumx /= a;
	sumy /= a;
	long long ans = MaxL;
	ans = min(ans, calc(sumx, sumy));
	ans = min(ans, calc(sumx + 1, sumy));
	ans = min(ans, calc(sumx, sumy + 1));
	ans = min(ans, calc(sumx + 1, sumy + 1));
	printf("Case #%d: %lld\n", Test, ans);
}

int main()
{
	int Test;
	scanf("%d", &Test);
	for (int i = 1; i <= Test; ++i)
		Work(i);
	return 0;
}