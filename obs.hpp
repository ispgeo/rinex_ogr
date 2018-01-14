
#ifndef OBS_HPP
#define OBS_HPP

#include "all.hpp"
#include "lib.hpp"
#include "nav.hpp"

class CDateTime
{

	double year, month, day, hour, minute, second, _seconds;

	public:

		CDateTime() { ; };
		CDateTime(const string & epoch_name);
    
		inline double seconds() const { return _seconds; };

};

class CSat
{

	const string _name;
	const Vector3d _x0;
	map<string, double> values;
	Vector4d _pos;
	Vector3d _a;
	double _d;

	string get_closest_measurements(const string & epoch_name, const CNav & nav);
	
	public:

		CSat(const string & name, const Vector3d & x0);

		// Расчитываем псевдодальность по http://www.navipedia.net/index.php/Ionosphere-free_Combination_for_Dual_Frequency_Receivers
		double iono_free(const string & key_1, const string & key_2);
		
		// Получаем псевдодальности для каждого спутника для несущих частот L1 и L2
		inline double iono_free_P1_P2() { return iono_free("P1", "P2"); };

		void eval(const CNav & nav, const string & epoch_name);
		double w();

		inline Vector4d pos() const { return _pos; };
		inline Vector3d a() const { return _a; };
		inline double d() const { return _d; };

};

class CEpoch
{

	const string _name;
	vector<CSat> _sats;

	public:

		CEpoch(const string & name, const Vector3d & x0);

		vector<CSat> sats(const CNav & nav);

};

class CObs
{

	public:

		CObs(const string & fname);

	    Vector3d x0();
		vector<CEpoch> epochs();

};

Vector3d shift(vector<CSat> & sats);

#endif

