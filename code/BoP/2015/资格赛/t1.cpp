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