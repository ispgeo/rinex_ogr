
#include "obs/sat.hpp"

const double c = 2.99792458E+8;

Vector3d CObsSat::shift(vector<CObsSat> & sats)
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
    
CObsSat::CObsSat(ifstream & fl, const string & name, const vector<string> & types_of_observ) :
	_name(name)
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

double CObsSat::iono_free(const string & key_1, const string & key_2)
{
	const double v_1 = values[key_1];
	const double v_2 = values[key_2];
    const double gamma = pow(1575.42 / 1227.6, 2);
	const double pr = (v_2 - gamma * v_1) / (1 - gamma);

    return pr;
}

void CObsSat::eval(CNav & nav, const string & epoch_name)
{
	const CDateTime tm = CDateTime::from_epoch_name(epoch_name);
	const string sat_epoch = get_closest_measurements(epoch_name, nav);

	if(sat_epoch == "")
	{
		_pos = Vector4d(0, 0, 0, 0);

		return;
	}

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

void CObsSat::fx(const Vector3d & x0)
{
    _d = sqrt(pow(_pos[0] - x0[0], 2) + pow(_pos[1] - x0[1], 2) + pow(_pos[2] - x0[2], 2));
	_a = Vector3d(- (_pos[0] - x0[0]) / _d, - (_pos[1] - x0[1]) / _d, - (_pos[2] - x0[2]) / _d);
}

string CObsSat::get_closest_measurements(const string & epoch_name, CNav & nav)
{
    const CDateTime tm = CDateTime::from_epoch_name(epoch_name);
    CDateTime max_sat_tm;
    string sat_epoch_name = "";

	for(auto & key : nav.epochs_name(_name))
	{
		const CDateTime key_tm = CDateTime::from_epoch_name(key);
        const double d = abs(key_tm.seconds() - tm.seconds());

        if(d <= 7200 && (sat_epoch_name == "" || max_sat_tm.seconds() < key_tm.seconds()))
		{
            sat_epoch_name = key;
            max_sat_tm = key_tm;
		}
	}

    return sat_epoch_name;
}

double CObsSat::w()
{
	const double pr = iono_free_P1_P2();
	const double l = pr + _pos[3] * c;
	const double w = l - _d;

	return w;
}

