
#ifndef LAYER_HPP
#define LAYER_HPP

#include "all.hpp"
#include "obs/obs.hpp"
#include "nav/nav.hpp"

class CLayer : public OGRLayer
{
	
	bool is_start_reading;
	int position_field_ind;
	shared_ptr<CObs> obs;
	shared_ptr<CNav> nav;
	shared_ptr<OGRFeatureDefn> feature_defn;
	OGRSpatialReference spatial_ref;
	OGRPoint approx_position, position;
	shared_ptr<CDateTime> first_obs, last_obs;

	void eval();
	
	public:
	
		CLayer(const string & fname);

		OGRFeature * GetNextFeature() override;

		inline void ResetReading() override { is_start_reading = true; };
		inline const char * GetName() override { return obs->marker_name().c_str(); };
		inline OGRFeatureDefn * GetLayerDefn() override { return feature_defn.get(); };
		inline int TestCapability(const char * cap) override { return FALSE; }; // Only read in this moment

};

#endif

