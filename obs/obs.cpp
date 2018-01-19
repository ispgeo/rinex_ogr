
#include "obs/obs.hpp"

CObs::CObs(const string & fname)
{
	string line;
	ifstream fl;
	vector<string> result;
	vector<string> types_of_observ;
	unique_ptr<regex> marker_name_re, approx_position_re, types_of_observ_re, antenna_re, reciever_re, first_obs_re, last_obs_re;
	regex head_end_re(".*END OF HEADER.*$");

	marker_name_re.reset(new regex("^(.{20}).*MARKER NAME"));
	antenna_re.reset(new regex("^(.{20})(.{20}).*ANT # / TYPE"));
	reciever_re.reset(new regex("^(.{20})(.{20})(.{20}).*REC # / TYPE / VERS"));
	approx_position_re.reset(new regex("[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*APPROX POSITION XYZ"));
	types_of_observ_re.reset(new regex(".*TYPES OF OBSERV.*$"));
	first_obs_re.reset(new regex("^(.{6})(.{6})(.{6})(.{6})(.{6})(.{13}).{5}(.{3}).*TIME OF FIRST OBS"));
	last_obs_re.reset(new regex("^(.{6})(.{6})(.{6})(.{6})(.{6})(.{13}).{5}(.{3}).*TIME OF LAST OBS"));
	
	fl.exceptions( ifstream::failbit | ifstream::badbit );
	fl.open(fname);
	fl.exceptions( ifstream::badbit );

	auto check_re = [ & ] (unique_ptr<regex> & re)
	{
		smatch _result;

		result.clear();

		if(re && regex_search(line, _result, * re))
		{
			unsigned v;

			re.reset();

			for(v = 1; v < _result.size(); v++)
				result.push_back(strip_space_in_start_and_end(_result[v]));

			return true;
		}

		return false;
	};

	while(fl.good())
	{
		line = next_line(fl);

		if(check_re(approx_position_re))
		{
			_x0[0] = stod(result[0]);
			_x0[1] = stod(result[1]);
			_x0[2] = stod(result[2]);
		}

		if(check_re(marker_name_re))
			_marker_name = result[0];

		if(check_re(antenna_re))
		{
			_antenna_model = result[0];
			_antenna_type = result[1];
		}

		if(check_re(reciever_re))
		{
			_reciever_model = result[0];
			_reciever_type = result[1];
			_reciever_version = result[2];
		}

		if(check_re(first_obs_re))
		{
			_first_obs = CDateTime(
					stod(result[0]),
					stod(result[1]),
					stod(result[2]),
					stod(result[3]),
					stod(result[4]),
					stod(result[5]));

			_first_obs_type = result[6];
		}

		if(check_re(last_obs_re))
		{
			_last_obs = CDateTime(
					stod(result[0]),
					stod(result[1]),
					stod(result[2]),
					stod(result[3]),
					stod(result[4]),
					stod(result[5]));

			_last_obs_type = result[6];
		}

		if(check_re(types_of_observ_re))
		{
			// TODO Multiline (num > 9) string

			unsigned v;
			const unsigned num = stoi(line.substr(0, 6));

			for(v = 0; v < num; v++)
			{
				const string type_of_observ = substr_and_del_space(line, (v + 1) * 6, 6);

				types_of_observ.push_back(type_of_observ);
			}
		}

		if(regex_match(line, head_end_re))
			break;
	}

	while(fl.good())
	{
		shared_ptr<CObsEpoch> epoch = CObsEpoch::load(fl, types_of_observ);

		if(epoch)
			_epochs.push_back(epoch);

		fl.peek(); // Return the next character without extracting it - enable check EOF for next good() without getline
	}
}

