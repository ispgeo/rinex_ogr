
#ifndef LAYER_HPP
#define LAYER_HPP

#include "all.hpp"

class CLayer : public OGRLayer
{
	
	bool is_start_reading;
	string layer_name;
	shared_ptr<OGRFeatureDefn> feature_defn;

	string marker_name;

	struct
	{
		double x, y, z;
	} approx_position;

	void load(const string path);
	
	public:
	
		CLayer(const string fname);
		~CLayer();

		void ResetReading() override;
		OGRFeature * GetNextFeature() override;
		inline const char * GetName() override { return layer_name.c_str(); };
		inline OGRFeatureDefn * GetLayerDefn() override { return feature_defn.get(); };
		inline int TestCapability(const char * cap) override { return FALSE; }; // TODO - only read in this moment

};

#endif

