
#include "layer.hpp"

CLayer::CLayer(const string fname)
{
	const string base_fname = CPLGetBasename(fname.c_str());
	const string path = CPLGetPath(fname.c_str());

	// #############################################################################
	// Spatial reference

	spatial_ref.importFromEPSG(6934);

	// #############################################################################
	// Feature definition

    auto p_feature_defn = new OGRFeatureDefn(base_fname.c_str());

	feature_defn.reset(p_feature_defn);
    feature_defn->Reference();
	feature_defn->SetGeomType(wkbPoint);

	{
		OGRGeomFieldDefn * field = feature_defn->GetGeomFieldDefn(0);
		field->SetSpatialRef(& spatial_ref);
	}

	{
		OGRGeomFieldDefn field("ApproxPosition", wkbPoint);
		field.SetSpatialRef(& spatial_ref);

		feature_defn->AddGeomFieldDefn(& field);
		approx_position_field_ind = feature_defn->GetGeomFieldIndex("ApproxPosition");
	}

	auto create_field = [ & ](const string field_name, const OGRFieldType type)
	{
		OGRFieldDefn field(field_name.c_str(), type);
		feature_defn->AddFieldDefn(& field);
	};

	create_field("MarkerName", OFTString);

	// ############################################################################ 
	// Rinex load

	load(path + "/" + base_fname);

	// #############################################################################
	// Other initialization
    
	SetDescription(base_fname.c_str());

	layer_name = marker_name;
	is_start_reading = true;
}

void CLayer::load(const string path)
{
	string line;
	ifstream fl;
	smatch result;
	unique_ptr<regex> marker_name_re, approx_position_re;

	marker_name_re.reset(new regex("[[:space:]]*([^[:space:]]+)[[:space:]]*MARKER NAME"));
	approx_position_re.reset(new regex("[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*([[:digit:]\\.]+)[[:space:]]*APPROX POSITION XYZ"));
	
	fl.exceptions( ifstream::failbit | ifstream::badbit );
	fl.open(path + ".17o"); // TODO Really 17o?
	fl.exceptions( ifstream::badbit );

	while(fl.good())
	{
		getline(fl, line);

		if(approx_position_re && regex_search(line, result, * approx_position_re))
		{
			approx_position.setX(stod(result[1]));
			approx_position.setY(stod(result[2]));
			approx_position.setZ(stod(result[3]));

			approx_position_re.reset();
		}

		if(marker_name_re && regex_search(line, result, * marker_name_re))
		{
			marker_name = result[1];

			marker_name_re.reset();
		}
	}

	eval(path);
}

void CLayer::eval(const string path)
{
	// TODO
	position.setX(1);
	position.setY(2);
	position.setZ(3);
}

OGRFeature * CLayer::GetNextFeature()
{
	if(is_start_reading)
	{
		OGRFeature * feature = new OGRFeature(feature_defn.get());

		is_start_reading = false;

		feature->SetFID(0);
		feature->SetGeometry(& position);
		feature->SetGeomField(approx_position_field_ind, & approx_position);
		feature->SetField("MarkerName", marker_name.c_str());

		return feature;
	}
	
	return NULL;
}

