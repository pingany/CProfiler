#include <vector>
#include <assert.h>
#include <ctype.h>
using namespace std;
#include "CommonAddress2Symbol.h"

#define ASSERT assert

#define SYMBOL_TABLE_STRAT_ID "Image Symbol Table"
#define SYMBOL_TABLE_END_ID "======="

/* If a function is compiled in thumb mode, 
it's address is a ODD value, we need to convert it a EVEN value(reset the end bit 1 to 0) 
*/
#define EVEN_ADDRESS(x)  ((x) & ~0x1U)

class ArmMapAddress2Symbol : public CommonAddress2Symbol
{
	const char* filename;
public:
	ArmMapAddress2Symbol(): filename("symbols.map") 
	{}
	void setFileName(const char* name) { filename = name; }
	BOOL init();
};

BOOL ArmMapAddress2Symbol::init()
{
	char line[1024];
	char symbol_buffer[1024], address_buffer[20];
	unsigned int address = 0;
	char * endp;
	FILE* fin = fopen(filename, "r");
	if (!fin)
		return FALSE;
	int start_id_len = strlen(SYMBOL_TABLE_STRAT_ID);
	int end_id_len = strlen(SYMBOL_TABLE_END_ID);
	BOOL in_symbol_regin = FALSE;
	while(NULL != fgets(line, sizeof(line), fin))
	{
		if(in_symbol_regin)
		{
			if (strstr(line, "ARM Code") || strstr(line, "Thumb Code"))
			{
				int ret = sscanf(line, "%s%s", symbol_buffer, address_buffer);
				ASSERT(ret == 2);
				if(ret != 2 || 0 != strncmp("0x", address_buffer, 2))
					continue;
				address = strtoul(address_buffer+2/*remove 0x prefix */, &endp, 16);
				addSymbol(EVEN_ADDRESS(address), symbol_buffer);
			}
		}
		else if(0 == strncmp(line, SYMBOL_TABLE_STRAT_ID, start_id_len))
		{
			in_symbol_regin = TRUE;
		}
		else if(0 == strncmp(line, SYMBOL_TABLE_END_ID, end_id_len) && in_symbol_regin)
		{
			in_symbol_regin = FALSE;
			break;
		}
	}
	sortSymbol();
	fclose(fin);
	return TRUE;
}

#ifdef BUILD_ARM_MAP_ADDRESS2SYMBOL
int main(int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <map file>\n", argv[0]);
		return 1;
	}
	ArmMapAddress2Symbol a2s;
	a2s.setFileName(argv[1]);
	if(! a2s.init())
	{
		fprintf(stderr, "Parse map file failed\n");
		return 1;
	}
	char line[1024];
	char * address, *address_end;
	while(NULL != fgets(line, sizeof(line), stdin))
	{
		address = strstr(line, "0x");
		if(address == 0)
			continue;
		address_end = address + 2;
		while(isalnum(*address_end))
			address_end++;
		*address_end = 0;
		char* symbol = a2s.getSymbol(EVEN_ADDRESS(strtoul(address+2, NULL, 16)));
		printf("%s\n", symbol ? symbol : "UnknownSymbol");
		if(symbol)
			a2s.freeSymbol(symbol);
	}
	return 0;
}
#endif
