#include <vector>
using namespace std;
#include "Address2Symbol.h"

class CommonAddress2Symbol : public Address2Symbol
{
public:
	struct Symbol
	{
		unsigned int address;
		char* symbol;
	};
private:
	vector<Symbol> * m_symbols;
	char* strdup(const char* s1);
public:
	CommonAddress2Symbol();
	~CommonAddress2Symbol();
	void addSymbol(int address, const char*symbol) 
	{
		Symbol s = {address, this->strdup(symbol)};
		m_symbols->push_back(s); 
	}
	void sortSymbol();
	BOOL init();
	char* getSymbol(U64 address);
	void freeSymbol(char *symbol);
};