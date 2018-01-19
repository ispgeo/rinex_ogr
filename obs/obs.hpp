
#ifndef OBS_HPP
#define OBS_HPP

#include "all.hpp"
#include "lib.hpp"
#include "obs/epoch.hpp"

class CObs
{
	
	string _marker_name, _antenna_model, _antenna_type,_reciever_model, _reciever_type, _reciever_version, _first_obs_type, _last_obs_type;
	CDateTime _first_obs, _last_obs;
	Vector3d _x0;
	vector< shared_ptr<CObsEpoch> > _epochs;

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
		inline vector< shared_ptr<CObsEpoch> > & epochs() { return _epochs; };

};

#endif

