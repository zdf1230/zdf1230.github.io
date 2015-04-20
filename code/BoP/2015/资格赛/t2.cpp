#include <cstdio>
#include <cstring>
using namespace std;

const int Maxn = 1000 + 10;
const int MOD = 100007;
char s[Maxn];
int f[Maxn][Maxn];

void Work(int Test)
{
	scanf("%s", s);
	int n = strlen(s);
	memset(f, 0, sizeof(f));
	for (int i = 0; i < n; ++i)
		f[i][i] = 1;
	for (int k = 1; k < n; ++k)
		for (int i = 0; i < n; ++i) {
			int j = i + k;
			if (j >= n) break;
			f[i][j] = (f[i][j] + f[i + 1][j] + f[i][j - 1] - f[i + 1][j - 1] + MOD) % MOD;
			if (s[i] == s[j])
				f[i][j] = (f[i][j] + f[i + 1][j - 1] + 1) % MOD;
		}
	printf("Case #%d: %d\n", Test, f[0][n - 1]);
}

int main()
{
	int Test;
	scanf("%d", &Test);
	for (int i = 1; i <= Test; ++i)
		Work(i);
	return 0;
}