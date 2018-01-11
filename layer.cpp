
#include "layer.hpp"

CLayer::CLayer(const string fname)
{
	const string base_fname = CPLGetBasename(fname.c_str());
	const string path = CPLGetPath(fname.c_str());
    auto p_feature_defn = new OGRFeatureDefn(base_fname.c_str());

    SetDescription(base_fname.c_str());

	feature_defn.reset(p_feature_defn);
    feature_defn->Reference();
    feature_defn->SetGeomType(wkbMultiPoint);

    OGRFieldDefn field("MarkerName", OFTString);
    feature_defn->AddFieldDefn(& field);

	load(path + "/" + base_fname);

	layer_name = marker_name;
	is_start_reading = true;
}

CLayer::~CLayer()
{
	;
}

void CLayer::load(const string path)
{
	string line;
	ifstream fl(path + ".17o"); // TODO Really 17o?
	smatch result;
	unique_ptr<regex> marker_name_re(new regex("[[:space:]]*([^[:space:]]+)[[:space:]]*MARKER NAME"));
	unique_ptr<regex> approx_position_re(new regex("[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*APPROX POSITION XYZ"));

	if(! fl.is_open())
		// TODO exception
		throw 1;

	while(fl.good())
	{
		getline(fl, line);

		if(approx_position_re && regex_search(line, result, * approx_position_re))
		{
			approx_position.x = stod(result[1]);
			approx_position.y = stod(result[2]);
			approx_position.z = stod(result[3]);

			approx_position_re.reset();
		}

		if(marker_name_re && regex_search(line, result, * marker_name_re))
		{
			marker_name = result[1];

			marker_name_re.reset();
		}
	}
}

void CLayer::ResetReading()
{
	is_start_reading = true;
}

OGRFeature * CLayer::GetNextFeature()
{
	if(is_start_reading)
	{
		is_start_reading = false;

		OGRFeature * feature = new OGRFeature(feature_defn.get());

		feature->SetFID(0);
		feature->SetGeometryDirectly(new OGRPoint(approx_position.x, approx_position.y, approx_position.z));
		feature->SetField("MarkerName", marker_name.c_str());

		return feature;
	}
	
	return NULL;
}

