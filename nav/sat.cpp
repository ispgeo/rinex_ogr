
#include "nav/sat.hpp"

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
	
	auto load_BO = [ & ](const vector<string> names)
	{
		string line = next_line(fl);
		const unsigned num = 19, size = line.size();
		unsigned next = 3;

		replace(line.begin(), line.end(), 'D', 'E');

		for(auto & name : names)
		{
			if(next >= size || next + num > size)
				break;

			values[name] = stod(line.substr(next, num));

			next += num;
		}
	};

	load_BO({ "IODE", "Crs", "Deln", "Mo" });
	load_BO({ "Cuc", "Ecc", "Cus", "SqrtA" });
	load_BO({ "TOE", "Cic", "OMEGA", "Cis" });
	load_BO({ "Io", "Crc", "Omega", "OMEGA_DOT" });
	load_BO({ "IDOT", "L2_CC", "GPS_W", "L2_P" });
	load_BO({ "SV_Acc", "SV_Health", "TGD", "IODC" });
	load_BO({ "Trans_time", "Fit_int" });
}

