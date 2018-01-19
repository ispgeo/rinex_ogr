
#ifndef NAV_HPP
#define NAV_HPP

#include "all.hpp"
#include "lib.hpp"
#include "nav/sat.hpp"

class CNav
{

	vector<CNavSat> _sats;

	public:

		CNav(const string & fname);

		vector<string> epochs_name(const string & sat_name);
		CNavSat & sat(const string & epoch_name, const string & sat_name);

};

#endif

