
#include "obs/epoch.hpp"

shared_ptr<CObsEpoch> CObsEpoch::load(ifstream & fl, const vector<string> & types_of_observ)
{
	string line = next_line(fl);

	// ############################################################################ 
	// Epoch name

	const string name = name_from_date(line, 0, 11);

	// ############################################################################ 
	// Flag

	const int flag = stoi(line.substr(26, 3));

	if(flag)
	{
		const unsigned special_line_num = stoi(line.substr(29, 3));
		unsigned v;

		for(v = 0; v < special_line_num; v++)
			next_line(fl);

		return NULL;
	}

	// ############################################################################ 
	// Sat num

	const unsigned sat_num = stoi(line.substr(29, 3));

	// ############################################################################ 
	// Sat names

	unsigned sat_ind, line_pos = 32;
	vector<string> sat_name;

	for(sat_ind = 1; sat_ind <= sat_num; sat_ind ++)
	{
		sat_name.push_back(line.substr(line_pos, 3));

		if(sat_ind % 12)
			line_pos += 3;
		else if (sat_ind < sat_num)
		{
			line = next_line(fl);
			line_pos = 32;
		}
	}

	// ############################################################################
	// Sats
	
	shared_ptr<CObsEpoch> epoch(new CObsEpoch);

	epoch->_name = name;

	for(auto & sat : sat_name)
		epoch->_sats.push_back(CObsSat(fl, sat, types_of_observ));

	return epoch;
}

vector<CObsSat> CObsEpoch::sats(CNav & nav)
{
	vector<CObsSat> res_sats;

	for(auto & sat : _sats)
	{
		sat.eval(nav, _name);
        const Vector4d pos = sat.pos();

        if(! (pos[0] == 0.0 || isnan(pos[0])))
            res_sats.push_back(sat);
	}
    
    return res_sats;
}

