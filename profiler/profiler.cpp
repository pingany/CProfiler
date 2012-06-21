#include <stdio.h>
#include <map>
#include <stack>
#include <time.h>
#include <assert.h>
using namespace std;

#include "Address2Symbol.h"

#ifdef DEBUG_PROFILER
#define ASSERT assert
#else
#define ASSERT(...) 
#endif

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

static FuncInfo counts;
static stack<Frame> frames;
static int x = 0;
// static const Info empty_info = {0};

#define TICK()	(clock())

static void do_enter()
{
	FuncInfo::iterator iter = counts.find(x);
	if (iter != counts.end())
		iter->second.count ++;
	else
	{
		Info f = {1, 0};
		counts.insert(pair<int, Info>(x, f));
	}
	Frame f= {x, 0};
	frames.push(f);
	Frame &topf = frames.top();
	/* Get tick in the end, above code may take much time, which makes profiler not exact */
	topf.tick = TICK();
}

static void do_exit()
{
	/* Get tick first, following code may take much time, which makes profiler not exact */
	int tick = TICK();
	Frame &f = frames.top();
	ASSERT(Exists(counts, f.func));
	counts[f.func].ms += tick - f.tick;
	frames.pop();
}

extern "C" void profiler_reset()
{
	counts.clear();
	while(!frames.empty())
		frames.pop();
}

extern "C" void profiler_print_info2(void* fileHandler)
{
	FILE* fout = (FILE*)fileHandler;
	FuncInfo::const_iterator iter;
	char* symbol = NULL;
	Address2Symbol a2s;
	a2s.init();
	for (iter = counts.begin(); iter != counts.end(); iter++)
	{
		symbol = a2s.getSymbol(iter->first);
		fprintf(fout, "Function %s 0x%x %d %dms\n", symbol ? symbol : "UnknownSymbol", iter->first, iter->second.count, iter->second.ms);
		if (symbol)
			a2s.freeSymbol(symbol);
	}
}

extern "C" void profiler_print_info(const char* filename)
{
	FILE* fout = fopen(filename, "w");
	if (!fout)
	{
		fprintf(stderr, "Open %s failed", filename);
		return;
	}
	profiler_print_info2(fout);
	fclose(fout);
}

#ifdef WIN32
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
#endif
