
#include "test/base.hpp"

void check(const bool cond, const string msg)
{
	if(cond)
	{
		perror(msg.c_str());

		exit(-1);
	}
}

void check_null(const void * ptr, const string msg)
{
	check(ptr == NULL, msg);
}

