
#include "obs.hpp"

const double c = 2.99792458E+8;

Vector3d shift(vector<SObsSat> & sats)
{
	const unsigned sats_num = sats.size();
	unsigned ind;
	VectorXd w(sats_num);
	MatrixXd a(sats_num, 3), shft;

	for(ind = 0; ind < sats_num; ind++)
	{
		const Vector3d ta = sats[ind].a();

		a(ind, 0) = ta[0];
		a(ind, 1) = ta[1];
		a(ind, 2) = ta[2];

		w[ind] = sats[ind].w();
	}

	auto aT = a.transpose();
	shft = (aT * a).inverse() * aT * w;

	return shft;
}

// ############################################################################ 

CDateTime::CDateTime(const string & epoch_name)
{
	const vector<string> t = split(epoch_name, ':');
    double yy, mm, hh;

	year = yy = 2000 + stod(t[0]);
	month = mm = stod(t[1]);
	day = stod(t[2]);
	hour = hh = stod(t[3]);
	minute = stod(t[4]);
	second = floor(stod(t[5]));

    hh += (minute / 60) + (second / 3600);

	if(mm <= 2)
	{
		yy--;
		mm += 12;
	}

    const double jd = floor(365.25 * (yy + 4716)) + floor(30.6001 * (mm + 1)) + day + (hh / 24) - 1537.5;
    const double a = floor(jd + 0.5);
    const double b = a + 1537;
    const double c = floor((b - 122.1) / 365.25);
    const double e = floor(365.25 * c);
    const double f = floor((b - e) / 30.6001);
    const double d = b - e - floor(30.6001 * f) + fmod(jd + 0.5, 1);
    const double day_of_week = fmod(floor(jd+0.5), 7);

	_seconds = (fmod(d, 1) + day_of_week + 1) * 86400;
}

// ############################################################################ 
    
SObsSat::SObsSat(ifstream & fl, const string & name, const Vector3d & x0, const vector<string> & types_of_observ) :
	_name(name), _x0(x0)
{
	string line;
	const unsigned num = types_of_observ.size();
	unsigned ind, ind_in_line;

	for(ind = 0, ind_in_line = 0; ind < num; ind++)
	{
		if(! (ind % 5))
		{
			line = next_line(fl);
			ind_in_line = 0;
		}

		string _value = substr_and_del_space(line, ind_in_line, 16);

		if(_value.empty())
			_value = "nan";

		values[types_of_observ[ind]] = stod(_value);
		ind_in_line += 16;
	}
}

double SObsSat::iono_free(const string & key_1, const string & key_2)
{
	const double v_1 = values[key_1];
	const double v_2 = values[key_2];
    const double gamma = pow(1575.42 / 1227.6, 2);
	const double pr = (v_2 - gamma * v_1) / (1 - gamma);

    return pr;
}

void SObsSat::eval(CNav & nav, const string & epoch_name)
{
	const CDateTime tm(epoch_name);
	const string sat_epoch = get_closest_measurements(epoch_name, nav);

	if(sat_epoch == "")
		_pos = Vector4d(0, 0, 0, 0);
	else
	{
		// Declare Variables
		const double ecc = nav.sat(sat_epoch, _name).value("Ecc");
		const double A = pow(nav.sat(sat_epoch, _name).value("SqrtA"), 2);
		const double deln = nav.sat(sat_epoch, _name).value("Deln");
		const double TOE = nav.sat(sat_epoch, _name).value("TOE");
		const double M0 = nav.sat(sat_epoch, _name).value("Mo");
		const double a0 = nav.sat(sat_epoch, _name).value("SV_CLB");
		const double a1 = nav.sat(sat_epoch, _name).value("SV_CLD");
		const double a2 = nav.sat(sat_epoch, _name).value("SV_CLR");
		const double tgd = nav.sat(sat_epoch, _name).value("TGD");
		const double pr = iono_free_P1_P2();

		// Argument of the Perigee
		const double ohm = nav.sat(sat_epoch, _name).value("Omega");
		
		const double cus = nav.sat(sat_epoch, _name).value("Cus");
		const double crs = nav.sat(sat_epoch, _name).value("Crs");
		const double cis = nav.sat(sat_epoch, _name).value("Cis");
		const double cuc = nav.sat(sat_epoch, _name).value("Cuc");
		const double crc = nav.sat(sat_epoch, _name).value("Crc");
		const double cic = nav.sat(sat_epoch, _name).value("Cic");
		
		const double i0 = nav.sat(sat_epoch, _name).value("Io");
		const double IDOT = nav.sat(sat_epoch, _name).value("IDOT");
		const double OHM = nav.sat(sat_epoch, _name).value("OMEGA");
		const double OHMDOT = nav.sat(sat_epoch, _name).value("OMEGA_DOT");
		
		// Mean Motion
		const double mu = 3.986005e+14;
		const double n0 = sqrt(mu / (pow(A, 3)));

		// Corrected Mean Motion
		const double n = n0 + deln;
		const double traw = tm.seconds() - pr / c;

		const double half_week = 302400;
		const double week = 2 * half_week;
		const double d_tk = traw - TOE;
		double tk = d_tk;

		if(d_tk > half_week)
			tk -= week;
		else if(d_tk < -1 * half_week)
			tk += week;

		// Mean Anomoly
		const double Mk = M0 + n * tk;
		
		double Ek0 = Mk;
		double Ek = Mk + ecc * sin(Ek0);

		while(abs(Ek - Ek0) > 0.000000000001)
		{
			Ek0 = Ek;
			Ek = Mk + ecc * sin(Ek0);
		}

		const double F = -4.442807633e-10;
		const double dtr = F * ecc * sqrt(A) * sin(Ek);
	    const double dtcorr = a0 + a1 * tk + a2 * tk * tk + dtr - tgd;
		
		// Compute sat pos
		// Compute True Anomaly Components
		const double ta_num = sin(Ek) * sqrt(1 - pow(ecc, 2));
		const double ta_den = cos(Ek) - ecc;

		// Compute True Anomaly
		const double v_k = atan2(ta_num, ta_den);

		// Argument of the Latitude
		const double phi_k = v_k + ohm;

		// Correction to Argument of Latitude
		const double del_uk = cus * sin(2 * phi_k) + cuc * cos(2 * phi_k);

		// Correction to Radius
		const double del_rk = crs*sin(2 * phi_k) + crc * cos(2 * phi_k);

		// Correction to Inclination
		const double del_ik = cis * sin(2 * phi_k) + cic * cos(2 * phi_k);

		// Corrected Argument of Latitude
		const double uk = phi_k + del_uk;

		// Corrected Radius
		const double rk = A * (1 - ecc * cos(Ek)) + del_rk;

		// Corrected Inclination
		const double ik = i0 + del_ik + IDOT * tk;

		// Orbital Plan Positions
		const double tx0 = rk * cos(uk);
		const double ty0 = rk * sin(uk);

		// Corrected Longitude of ascending node
		const double OHMDOTe = 7.2921151467e-5;
		const double OMEGAK = OHM + tk * (OHMDOT - OHMDOTe) - OHMDOTe * TOE;

		// Earthfixed Coordinates
		const double xk = tx0 * cos(OMEGAK) - ty0 * cos(ik) * sin(OMEGAK);
		const double yk = tx0 * sin(OMEGAK) + ty0 * cos(ik) * cos(OMEGAK);
		const double zk = ty0 * sin(ik);

		_pos = Vector4d(xk, yk, zk, dtcorr);
	}

	// ############################################################################ 
    // Расстояние до спутника

    _d = sqrt(pow(_pos[0] - _x0[0], 2) + pow(_pos[1] - _x0[1], 2) + pow(_pos[2] - _x0[2], 2));
	_a = Vector3d(- (_pos[0] - _x0[0]) / _d, - (_pos[1] - _x0[1]) / _d, - (_pos[2] - _x0[2]) / _d);
}

string SObsSat::get_closest_measurements(const string & epoch_name, CNav & nav)
{
    const CDateTime tm(epoch_name);
    CDateTime max_sat_tm;
    string sat_epoch_name = "";

	for(auto & key : nav.epochs_name(_name))
	{
		const CDateTime key_tm(key);
        const double d = abs(key_tm.seconds() - tm.seconds());

        if(d <= 7200 && (sat_epoch_name == "" || max_sat_tm.seconds() < key_tm.seconds()))
		{
            sat_epoch_name = key;
            max_sat_tm = key_tm;
		}
	}

    return sat_epoch_name;
}

double SObsSat::w()
{
	const double pr = iono_free_P1_P2();
	const double l = pr + _pos[3] * c;
	const double w = l - _d;

	return w;
}

// ############################################################################ 

CObsEpoch::CObsEpoch(ifstream & fl, const Vector3d & x0, const vector<string> & types_of_observ)
{
	string line = next_line(fl);

	// ############################################################################ 
	// Epoch name

	_name = name_from_date(line, 0, 11);

	// ############################################################################ 
	// Flag

	_flag = stoi(line.substr(26, 3));

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
		else
		{
			line = next_line(fl);
			line_pos = 32;
		}
	}

	// ############################################################################
	// Sats

	for(auto & sat : sat_name)
		_sats.push_back(SObsSat(fl, sat, x0, types_of_observ));
}

vector<SObsSat> CObsEpoch::sats(CNav & nav)
{
	vector<SObsSat> res_sats;

	for(auto & sat : _sats)
	{
		sat.eval(nav, _name);
        const Vector4d pos = sat.pos();

        if(! (pos[0] == 0.0 || isnan(pos[0])))
            res_sats.push_back(sat);
	}
    
    return res_sats;
}

// ############################################################################ 

CObs::CObs(const string & fname)
{
	string line;
	ifstream fl;
	smatch result;
	vector<string> types_of_observ;
	unique_ptr<regex> marker_name_re, approx_position_re, types_of_observ_re;
	regex head_end_re(".*END OF HEADER.*$");

	marker_name_re.reset(new regex("[[:space:]]*([^[:space:]]+)[[:space:]]*MARKER NAME"));
	approx_position_re.reset(new regex("[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*APPROX POSITION XYZ"));
	types_of_observ_re.reset(new regex(".*TYPES OF OBSERV.*$"));
	
	fl.exceptions( ifstream::failbit | ifstream::badbit );
	fl.open(fname);
	fl.exceptions( ifstream::badbit );

	while(fl.good())
	{
		line = next_line(fl);

		if(approx_position_re && regex_search(line, result, * approx_position_re))
		{
			_x0[0] = stod(result[1]);
			_x0[1] = stod(result[2]);
			_x0[2] = stod(result[3]);
		}

		if(marker_name_re && regex_search(line, result, * marker_name_re))
		{
			_marker_name = result[1];

			marker_name_re.reset();
		}

		if(types_of_observ_re && regex_match(line, * types_of_observ_re))
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
		_epochs.push_back(CObsEpoch(fl, _x0, types_of_observ));

		fl.peek(); // Return the next character without extracting it - enable check EOF for next good() without getline
	}
}

