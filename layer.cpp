
#include "layer.hpp"

CLayer::CLayer(const string & fname)
{
	const string base_fname = CPLGetBasename(fname.c_str());
	const string path = fname.substr(0, fname.length() - 1);

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

	auto create_field = [ & ](const string & field_name, const OGRFieldType & type)
	{
		OGRFieldDefn field(field_name.c_str(), type);
		feature_defn->AddFieldDefn(& field);
	};

	create_field("MarkerName", OFTString);

	// ############################################################################ 
	// Rinex load

	load(path);

	// #############################################################################
	// Other initialization
    
	SetDescription(base_fname.c_str());

	layer_name = marker_name;
	is_start_reading = true;
}

void CLayer::load(const string & path)
{
	unsigned v;
	CObs obs(path + "o");
	CNav nav(path + "N");
    const Vector3d x0 = obs.x0();
	Vector3d x(x0), x_result(0, 0, 0);

    // Эпохи
    vector<CObsEpoch> & epochs = obs.epochs(); // TODO const?

	for(auto & epoch : epochs)
	{
        // Ищем спутники, для которых известно положение в данную эпоху
		vector<SObsSat> sats = epoch.sats(nav);

		if(sats.size() < 4)
			continue;

		for(v = 0; v < 5; v++)
		{
			// TODO reeval sat pos
			MatrixXd shft = shift(sats);

			if(! isnan(shft(0, 0)))
				x += shft;
		}

		x_result += x;
	}

	x_result /= epochs.size();

	approx_position.setX(x0[0]);
	approx_position.setY(x0[1]);
	approx_position.setZ(x0[2]);

	position.setX(x_result[0]);
	position.setY(x_result[1]);
	position.setZ(x_result[2]);

	marker_name = obs.marker_name();
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

