---
layout: post
title: "2015 Beauty of Programming qualifier"
author: Zdf
tag: BoP
category: BoP
date: 2015-04-20 12:25:47 -0700
comments: true
---

####2015编程之美资格赛

每次这种大型比赛都必跪啊= =今年又来参加了，由于好久没写过代码了，就把资格赛的三个题都写了。

	

{% highlight cpp %}

#include <cstdio>
#include <string>
#include <iostream>

using namespace std;

const string str[] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November" , "December"};

int calc(int month, int day, int year)
{
	int ans = year / 4 - year / 100 + year / 400;
	if (month < 3 && (year % 4 == 0 && year % 100 != 0 || year % 400 == 0))
		--ans;
	return ans;
}

void Work(int Test)
{
	string s;
	int month, day, year;
	cin >> s;
	for (int i = 1; i <= 12; ++i)
		if (s == str[i])
			month = i;
	scanf("%d, %d", &day, &year);
	int ans = calc(month, day, year);
	cin >> s;
	for (int i = 1; i <= 12; ++i)
		if (s == str[i])
			month = i;
	scanf("%d, %d", &day, &year);
	ans = calc(month, day, year)- ans;
	if (month == 2 && day == 29) ++ans;
	printf("Case #%d: %d\n", Test, ans);
}

int main()
{
	int Test;
	scanf("%d", &Test);
	for (int i = 1; i <= Test; ++i)
		Work(i);
	return 0;
}


{% endhighlight %}

第一题就是算出某一天到之前有多少个2.29,然后相减就行。

{% highlight cpp %}

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

{% endhighlight %}

第二题就是个DP= =

{% highlight cpp %}

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

{% endhighlight %}

第三题还比较有意思。很多人用三分来写，而我没有。

因为x轴y轴相对独立，我们先考虑一个象限。

对于一个象限，我们可以列出式子，然后打开，求导，求最值，可以发现，最优值是坐标的平均值，因为可能不整除，我们假定最优值可以为`x0`或`x0 + 1`。y轴同理。所以对于欧几里得距离的最优值就在这四个点中。然后我们还可以发现，如果坐标移动1，欧几里得距离的影响会比曼哈顿距离的影响要大，所以最优值还在这四个点中，分别算出，取最优即可。