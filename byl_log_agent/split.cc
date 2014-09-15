#include "split.h"

int split(const string& src, const char s, vector<string> &result)
{
	unsigned int i;
	char c;
	string item;
	const char *buf = src.c_str();
	
	result.clear();	
	if(src.empty())
		return -1;

	for (i = 0; i < src.length(); i++)
	{
		c = *buf++;
		if (c == s)
		{
			result.insert(result.end(), item);
			item.clear();
			continue;
		}
		item.append(1, c);
	}
	
	result.insert(result.end(), item);
	
	return 0;
}
