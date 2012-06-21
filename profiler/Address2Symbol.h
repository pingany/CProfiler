
#ifndef ADDRESS_2_SYMBOL_H
#define ADDRESS_2_SYMBOL_H

#ifdef WIN32
typedef unsigned __int64 U64;
#elif defined( __GNUC__)
typedef unsigned long long U64;
#endif

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

class Address2Symbol
{
public:
	BOOL init();
	char* getSymbol(U64 address);
	void freeSymbol(char* symbol);
};

#endif /* ADDRESS_2_SYMBOL_H */
