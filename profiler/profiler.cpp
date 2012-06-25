#include <stdio.h>
#include <stack>
#include <time.h>
#include <assert.h>
using namespace std;

#ifdef NO_HASH_MAP
#	define MAP map
#	include <map>
#else
#	define MAP hash_map
#	ifdef __GNUC__
#		include <ext/hash_map>
		using namespace __gnu_cxx;
#	elif defined(_MSC_VER)
#		include <hash_map>
		using namespace stdext;
#	endif
#endif /* NO_HASH_MAP */

#ifdef MULTITHREAD_SUPPORT
#	ifdef __GNUC__
#		define TLS __thread
#	elif defined(_MSC_VER)
#		define TLS __declspec(thread)
#	else
#		error "don't support multithread"
#	endif
#
#   define AUTOLOCK(locker)	/* FIXME */
#else
#	define TLS
#   define AUTOLOCK(locker)		
#endif /* MULTITHREAD_SUPPORT */

#include "Address2Symbol.h"
#include "profiler.h"

#ifdef DEBUG_PROFILER
#define ASSERT assert
#define Exists(x, y) (x.find(y) != x.end())
#else
#define ASSERT(...) 
#endif

#undef uint
typedef unsigned int uint;

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

typedef MAP<uint, Info> FuncInfo;

static int initialized = 0;
static FuncInfo counts;
static TLS stack<Frame> frames;
#ifdef _MSC_VER
static TLS uint s_current_function = 0;
#endif
// static const Info empty_info = {0};

#ifndef TICK
#define TICK	(clock())
#endif

static void do_enter(uint current_function)
{
#ifndef MULTITHREAD_SUPPORT
	if (!initialized)
	{
		profiler_reset();
	}
#endif
	{
		AUTOLOCK(COUNTS_LOCKER);
		FuncInfo::iterator iter = counts.find(current_function);
		if (iter != counts.end())
			iter->second.count ++;
		else
		{
			Info info = {1, 0};
			counts.insert(pair<uint, Info>(current_function, info));
		}
	}
	Frame frame = {current_function, 0};
	frames.push(frame);
	Frame &topf = frames.top();
	/* Get tick in the end, above code may take much time, which makes profiler not exact */
	topf.tick = TICK;
}

static void do_exit()
{
	/* Get tick first, following code may take much time, which makes profiler not exact */
	uint tick = TICK;
	ASSERT(initialized);
	Frame &f = frames.top();
	{
		AUTOLOCK(COUNTS_LOCKER);
		ASSERT(Exists(counts, f.func));
		counts[f.func].ms += tick - f.tick;
	}
	frames.pop();
}

extern "C" void profiler_reset()
{
	counts.clear();
	// while(!frames.empty())
	// 	frames.pop();
	initialized = 1;
}

extern "C" void profiler_print_info2(void* fileHandler)
{
	FILE* fout = (FILE*)fileHandler;
	FuncInfo::const_iterator iter;
	char* symbol = NULL;
#ifndef NO_SYMBOL
	Address2Symbol *a2s;
	if(0 != Address2Symbol::create(&a2s))
	{
		fprintf(stderr, "%s\n", "Address2Symbol::create failed");
		return;
	}
	if(!a2s->init())
	{
		fprintf(stderr, "%s\n", "Address2Symbol::init failed");		
		return;
	}
#endif
	{
		AUTOLOCK(COUNTS_LOCKER);
		for (iter = counts.begin(); iter != counts.end(); iter++)
		{
#ifndef NO_SYMBOL
			symbol = a2s->getSymbol(iter->first);
#endif
			fprintf(fout, "Function %s 0x%08x %d %dms\n", symbol ? symbol : "UnknownSymbol", iter->first, iter->second.count, iter->second.ms);
#ifndef NO_SYMBOL 
			if (symbol)
				a2s->freeSymbol(symbol);
#endif
		}
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

#ifdef _MSC_VER
extern "C" void __declspec(naked) _cdecl _penter( void ) {
	_asm {
		/* Get the value in the top of stack. */
		pop s_current_function
		push s_current_function
		push eax
		push ebx
		push ecx
		push edx
		push ebp
		push edi
		push esi
	}
	do_enter(s_current_function);
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

#ifdef __GNUC__

#define DUMP(func, call) \
        printf("%s: func = 0x%08x, caller = 0x%08x\n", __FUNCTION__, func, call)

extern "C" void __cyg_profile_func_enter(void *this_func, void *call_site)
{
	// DUMP(this_func, call_site);
	do_enter((uint)this_func);
}

extern "C" void __cyg_profile_func_exit(void *this_func, void *call_site)
{
	// DUMP(this_func, call_site);
	do_exit();
}

#endif /* __GNUC__ */