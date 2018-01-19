
#ifndef NAV_SAT_HPP
#define NAV_SAT_HPP

#include "all.hpp"
#include "lib.hpp"

class CNavSat
{

	string _name, _epoch;
	map<string, double> values;

	public:

		CNavSat(ifstream & fl);

		inline string name() { return _name; };
		inline string epoch() { return _epoch; };
		inline double value(const string & key) { return values[key]; };

};

#endif

