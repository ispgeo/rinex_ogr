
#ifndef OBS_EPOCH_HPP
#define OBS_EPOCH_HPP

#include "all.hpp"
#include "lib.hpp"
#include "obs/sat.hpp"
#include "nav/nav.hpp"

class CObsEpoch
{

	string _name;
	vector<CObsSat> _sats;

	public:

		vector<CObsSat> sats(CNav & nav);

		inline string name() const { return _name; };

		static shared_ptr<CObsEpoch> load(ifstream & fl, const vector<string> & types_of_observ);

};

#endif

