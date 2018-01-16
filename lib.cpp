
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

	res.push_back(next);

	return res;
}

string next_line(ifstream & fl)
{
	string line;

	getline(fl, line);

	line.erase(remove(line.begin(), line.end(), '\n'), line.end());
	line.erase(remove(line.begin(), line.end(), '\r'), line.end());

	return line;
}

string substr_and_del_space(const string & line, const unsigned from, const unsigned num)
{
	string res;
	
	try
	{
		res = line.substr(from, num);
	}
	catch(const out_of_range & err)
	{
		try
		{
			res = line.substr(from);
		}
		catch(const out_of_range & err)
		{
			res = "";
		}
	}

	res.erase(remove(res.begin(), res.end(), ' '), res.end());
	res.erase(remove(res.begin(), res.end(), '\t'), res.end());

	return res;
}

string name_from_date(const string & line, const unsigned begin, const unsigned sec_size)
{
	const string year = substr_and_del_space(line, begin, 3);
	const string month = substr_and_del_space(line, begin + 3, 3);
	const string day = substr_and_del_space(line, begin + 6, 3);
	const string hour = substr_and_del_space(line, begin + 9, 3);
	const string minute = substr_and_del_space(line, begin + 12, 3);
	const string second = substr_and_del_space(line, begin + 15, sec_size);
	
	const string name = year + ":" + month + ":" + day + ":" + hour + ":" + minute + ":" + second;

	return name;
}

