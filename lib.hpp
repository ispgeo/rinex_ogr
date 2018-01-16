
#ifndef LIB_HPP
#define LIB_HPP

#include "all.hpp"

vector<string> split(const string str, const char delim);
string next_line(ifstream & fl);
string substr_and_del_space(const string & line, const unsigned from, const unsigned num);
string name_from_date(const string & line, const unsigned begin, const unsigned sec_size);

#endif

