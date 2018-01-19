
#include "nav/nav.hpp"

CNav::CNav(const string & fname)
{
	string line;
	ifstream fl;
	regex head_end_re(".*END OF HEADER.*$");

	fl.exceptions( ifstream::failbit | ifstream::badbit );
	fl.open(fname);
	fl.exceptions( ifstream::badbit );

	while(fl.good())
	{
		line = next_line(fl);

		if(regex_match(line, head_end_re))
			break;
	}

	while(fl.good())
	{
		_sats.push_back(CNavSat(fl));

		fl.peek(); // Return the next character without extracting it - enable check EOF for next good() without getline
	}
}

vector<string> CNav::epochs_name(const string & sat_name)
{
	vector<string> epochs;

	for(auto & sat : _sats)
		if(sat.name() == sat_name)
			epochs.push_back(sat.epoch());

	return epochs;
}

CNavSat & CNav::sat(const string & epoch_name, const string & sat_name)
{
	for(auto & sat : _sats)
		if(sat.name() == sat_name && sat.epoch() == epoch_name)
			return sat;

	throw runtime_error("Error: sattelite " + sat_name + " not found in epoch " + epoch_name);
}

