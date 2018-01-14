
#include "lib.hpp"

// TODO Very bad reliase
vector<string> split(const string str, const char delim)
{
	unsigned ind;
	string next = "";
	vector<string> res;

	for(ind = 0; ind < str.size(); ind++)
	{
		if(str[ind] == delim)
		{
			res.push_back(next);
			next = "";
		}
		else
			next += str[ind];
	}

	return res;
}

