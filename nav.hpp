
#ifndef NAV_HPP
#define NAV_HPP

#include "all.hpp"

class CNav
{

	public:

		CNav(const string & fname);

		vector<string> epochs(const string & sat_name) const;
		double field(const string & sat_name, const string & sat_epoch, const string & field_name) const;

};

#endif

