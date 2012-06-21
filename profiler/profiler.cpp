#include <stdio.h>
#include <map>
#include <stack>
#include <time.h>
#include <assert.h>
using namespace std;

#define ASSERT assert
#undef uint
typedef unsigned int uint;

#define FuncInfo map<int, Info>

struct Info
{
	uint count;
	uint ms;
};

struct Frame
{
	uint func;
	uint tick;
};

FuncInfo counts;
stack<Frame> frames;

int x = 0;
const Info empty_info = {0};
#define Exists(counts, x) (counts.find(x) != counts.end())
#define TICK()	(clock())

void do_enter()
{
	FuncInfo::iterator iter = counts.find(x);
	if (iter != counts.end())
		iter->second.count ++;
	else
	{
		Info f = {1, 0};
		counts.insert(pair<int, Info>(x, f));
	}
	Frame f= {x, TICK()};
	frames.push(f);
}

void do_exit()
{
	Frame f = frames.top();
	ASSERT(Exists(counts, f.func));
	counts[f.func].ms += TICK() - f.tick;
}

void print_info()
{
	FuncInfo::const_iterator iter;
	for (iter = counts.begin(); iter != counts.end(); iter++)
	{
		printf("Function 0x%x %d %d\n", iter->first, iter->second.count, iter->second.ms);
	}
}

extern "C" void __declspec(naked) _cdecl _penter( void ) {
	_asm {
		pop x
			push x
			push eax
			push ebx
			push ecx
			push edx
			push ebp
			push edi
			push esi
	}
	do_enter();
	_asm {
		pop esi
			pop edi
			pop ebp
			pop edx
			pop ecx
			pop ebx
			pop eax
			ret
	}
}

extern "C" void __declspec(naked) _cdecl _pexit( void )
{
	_asm {
		push eax
			push ebx
			push ecx
			push edx
			push ebp
			push edi
			push esi
	}

	do_exit();

	_asm {
		pop esi
			pop edi
			pop ebp
			pop edx
			pop ecx
			pop ebx
			pop eax
			ret
	}
}
