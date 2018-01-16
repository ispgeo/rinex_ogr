
#ifndef LAYER_HPP
#define LAYER_HPP

#include "all.hpp"
#include "obs.hpp"
#include "nav.hpp"

class CLayer : public OGRLayer
{
	
	bool is_start_reading;
	int approx_position_field_ind;
	string layer_name;
	shared_ptr<OGRFeatureDefn> feature_defn;
	OGRSpatialReference spatial_ref;

	OGRPoint approx_position, position;
	string marker_name;

	void load(const string & path);
	
	public:
	
		CLayer(const string & fname);

		OGRFeature * GetNextFeature() override;

		inline void ResetReading() override { is_start_reading = true; };
		inline const char * GetName() override { return layer_name.c_str(); };
		inline OGRFeatureDefn * GetLayerDefn() override { return feature_defn.get(); };
		inline int TestCapability(const char * cap) override { return FALSE; }; // TODO - only read in this moment

};

#endif

