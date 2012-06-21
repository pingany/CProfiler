#include <stdio.h>
#include "profiler/profiler.h"

int add(int x, int y)
{
	return x + y;
}

int add1(int x, int y)
{
	return add(x, y) + add(x, y);
}

void test1()
{
	int N = 3000;
	int sum = 0;
	for(int i =0 ; i< N ; i++)
		sum += add1(i, i+1);
	printf("sum = %d\n", sum);
}

int __attribute__((__no_instrument_function__)) main();
int main()
{
	//profiler_reset();
	//freopen("profile_result.txt", "w", stdout);
	test1();
	
	profiler_print_info2(stdout);
	return 0;
}

