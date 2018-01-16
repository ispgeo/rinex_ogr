
#include "nav.hpp"

CNavSat::CNavSat(ifstream & fl)
{
	string line;

	// ############################################################################ 
	// PRN
	
	line = next_line(fl);
	replace(line.begin(), line.end(), 'D', 'E');

	_name = "G" + line.substr(0, 2);
	_epoch = name_from_date(line, 2, 5);
	values["SV_CLB"] = stod(line.substr(22, 19));
	values["SV_CLD"] = stod(line.substr(41, 19));
	values["SV_CLR"] = stod(line.substr(60, 19));

	// ############################################################################ 
	// BO
	
	auto load_BO = [ & ](const string & name_1, const string & name_2, const string & name_3, const string & name_4)
	{
		string line = next_line(fl);
		replace(line.begin(), line.end(), 'D', 'E');

		values[name_1] = stod(line.substr(3, 19));
		values[name_2] = stod(line.substr(22, 19));

		if(! name_3.empty())
			values[name_3] = stod(line.substr(41, 19));

		if(! name_4.empty())
			values[name_4] = stod(line.substr(60, 19));
	};

	load_BO("IODE", "Crs", "Deln", "Mo");
	load_BO("Cuc", "Ecc", "Cus", "SqrtA");
	load_BO("TOE", "Cic", "OMEGA", "Cis");
	load_BO("Io", "Crc", "Omega", "OMEGA_DOT");
	load_BO("IDOT", "L2_CC", "GPS_W", "L2_P");
	load_BO("SV_Acc", "SV_Health", "TGD", "IODC");
	load_BO("Trans_time", "Fit_int", "", "");
}

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

	throw 0; // TODO Bad
}

