
#ifndef NAV_HPP
#define NAV_HPP

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

class CNav
{

	vector<CNavSat> _sats;

	public:

		CNav(const string & fname);

		vector<string> epochs_name(const string & sat_name);
		CNavSat & sat(const string & epoch_name, const string & sat_name);

};

#endif

