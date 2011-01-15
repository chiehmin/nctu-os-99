#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;


int main()
{
	int cases;
	scanf("%d", &cases);
	while (getchar() != '\n');

	string s;
	while (cases--) {
		getline(cin, s);
		stringstream ss(s);
		int mid;
		ss >> mid;

		int small = mid, big = mid;

		int d;
		while (ss >> d) {
			if (d<small) small = d;
			if (d>big) big = d;
		}
		printf("%d\n", min(mid-small, big-mid)+big-small);
	}


}

