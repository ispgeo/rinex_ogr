
#ifndef OBS_HPP
#define OBS_HPP

#include "all.hpp"
#include "lib.hpp"
#include "nav.hpp"

class CObsSat
{

	const string _name;
	map<string, double> values;
	Vector4d _pos;
	Vector3d _a;
	double _d;

	string get_closest_measurements(const string & epoch_name, CNav & nav);
	
	public:

		CObsSat(ifstream & fl, const string & name, const vector<string> & types_of_observ);

		// Расчитываем псевдодальность по http://www.navipedia.net/index.php/Ionosphere-free_Combination_for_Dual_Frequency_Receivers
		double iono_free(const string & key_1, const string & key_2);
		
		// Получаем псевдодальности для каждого спутника для несущих частот L1 и L2
		inline double iono_free_P1_P2() { return iono_free("P1", "P2"); };

		void eval(CNav & nav, const string & epoch_name);
		void fx(const Vector3d & x0);
		double w();

		inline Vector4d pos() const { return _pos; };
		inline Vector3d a() const { return _a; };
		inline double d() const { return _d; };
		inline string name() const { return _name; };

};

class CObsEpoch
{

	string _name;
	unsigned _flag;
	vector<CObsSat> _sats;

	public:

		CObsEpoch(ifstream & fl, const vector<string> & types_of_observ);

		vector<CObsSat> sats(CNav & nav);

};

class CObs
{
	
	string _marker_name, _antenna_model, _antenna_type,_reciever_model, _reciever_type, _reciever_version, _first_obs_type, _last_obs_type;
	CDateTime _first_obs, _last_obs;
	Vector3d _x0;
	vector<CObsEpoch> _epochs;

	public:

		CObs(const string & fname);

		inline const string & marker_name() { return _marker_name; };
		inline const string & antenna_model() { return _antenna_model; };
		inline const string & antenna_type() { return _antenna_type; };
		inline const string & reciever_model() { return _reciever_model; };
		inline const string & reciever_type() { return _reciever_type; };
		inline const string & reciever_version() { return _reciever_version; };
		inline const CDateTime & first_obs() { return _first_obs; };
		inline const CDateTime & last_obs() { return _last_obs; };
		inline const string & first_obs_type() { return _first_obs_type; };
		inline const string & last_obs_type() { return _last_obs_type; };
	    inline const Vector3d & x0() { return _x0; };
		inline vector<CObsEpoch> & epochs() { return _epochs; };

};

Vector3d shift(vector<CObsSat> & sats);

#endif

