
#include "lib.hpp"

// TODO Very bad
vector<string> split(const string & str, const char delim)
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

string strip_space_in_start_and_end(const string & line)
{
	const string res_1 = regex_replace(line, regex("^[[:space:]]+"), "");
	const string res = regex_replace(res_1, regex("[[:space:]]+$"), "");

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

// ############################################################################ 

CDateTime::CDateTime(const double y, const double m, const double d, const double h, const double mn, const double s) :
	_year(y), _month(m), _day(d), _hour(h), _minute(mn), _second(s)
{
	double yy = _year;
	double mm = _month;
	double hh = _hour;

    hh += (_minute / 60) + (_second / 3600);

	if(mm <= 2)
	{
		yy--;
		mm += 12;
	}

    const double jd = floor(365.25 * (yy + 4716)) + floor(30.6001 * (mm + 1)) + _day + (hh / 24) - 1537.5;
    const double a = floor(jd + 0.5);
    const double b = a + 1537;
    const double c = floor((b - 122.1) / 365.25);
    const double e = floor(365.25 * c);
    const double f = floor((b - e) / 30.6001);
    const double dbe = b - e - floor(30.6001 * f) + fmod(jd + 0.5, 1);
    const double day_of_week = fmod(floor(jd+0.5), 7);

	_seconds = (fmod(dbe, 1) + day_of_week + 1) * 86400;
}

CDateTime CDateTime::from_epoch_name(const string & epoch_name)
{
	const vector<string> t = split(epoch_name, ':');
	const double year = 2000 + stod(t[0]);
	const double month = stod(t[1]);
	const double day = stod(t[2]);
	const double hour = stod(t[3]);
	const double minute = stod(t[4]);
	const double second = floor(stod(t[5]));
	
	CDateTime dt(year, month, day, hour, minute, second);

	return dt;
}

