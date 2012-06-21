#include <stdio.h>
#include "profiler/profiler.h"

int add1(int x, int y)
{
	return x+ y;
}

void test1()
{
	int N = 10001;
	int sum = 0;
	for(int i =0 ; i< N ; i++)
		sum += add1(i, i+1);
	printf("sum = %d\n", sum);
}

int main()
{
	//freopen("profile_result.txt", "w", stdout);
	test1();
	print_info();
	return 0;
}

