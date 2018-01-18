
#ifndef LIB_HPP
#define LIB_HPP

#include "all.hpp"

class CDateTime
{

	double _year, _month, _day, _hour, _minute, _second, _seconds;

	public:

		CDateTime() { ; };
		CDateTime(const double y, const double m, const double d, const double h, const double mn, const double s);

		static CDateTime from_epoch_name(const string & epoch_name);
    
		inline double year() const { return _year; };
		inline double month() const { return _month; };
		inline double day() const { return _day; };
		inline double hour() const { return _hour; };
		inline double minute() const { return _minute; };
		inline double second() const { return _second; };
		inline double seconds() const { return _seconds; };

};

vector<string> split(const string & str, const char delim);
string next_line(ifstream & fl);
string substr_and_del_space(const string & line, const unsigned from, const unsigned num);
string strip_space_in_start_and_end(const string & line);
string name_from_date(const string & line, const unsigned begin, const unsigned sec_size);
string find_file(const string & path, const vector<string> exts);

#endif

