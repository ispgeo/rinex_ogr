
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
		OGRGeomFieldDefn field("Position", wkbPoint);
		field.SetSpatialRef(& spatial_ref);

		feature_defn->AddGeomFieldDefn(& field);
		position_field_ind = feature_defn->GetGeomFieldIndex("Position");
	}

	auto create_field = [ & ](const string & field_name, const OGRFieldType & type)
	{
		OGRFieldDefn field(field_name.c_str(), type);
		feature_defn->AddFieldDefn(& field);
	};

	create_field("MarkerName", OFTString);
	create_field("AntennaModel", OFTString);
	create_field("AntennaType", OFTString);
	create_field("RecieverModel", OFTString);
	create_field("RecieverType", OFTString);
	create_field("RecieverVersion", OFTString);
	create_field("FirstObsDateTime", OFTDateTime);
	create_field("LastObsDateTime", OFTDateTime);
	create_field("FirstObsTimeType", OFTString);
	create_field("LastObsTimeType", OFTString);

	// ############################################################################ 
	// Rinex load and eval

	obs.reset(new CObs(path + "o"));
	nav.reset(new CNav(path + "N"));

	eval();

	// #############################################################################
	// Other initialization
    
	SetDescription(base_fname.c_str());

	is_start_reading = true;
}

void CLayer::eval()
{
	unsigned v;
    const Vector3d x0 = obs->x0();
	Vector3d x(x0), x_result(0, 0, 0);

    // Эпохи
    vector<CObsEpoch> & epochs = obs->epochs();

	for(auto & epoch : epochs)
	{
        // Ищем спутники, для которых известно положение в данную эпоху
		vector<CObsSat> sats = epoch.sats(* nav);

		if(sats.size() < 4)
			continue;

		for(v = 0; v < 5; v++)
		{
			for(auto & sat : sats)
				sat.fx(x);

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
}

OGRFeature * CLayer::GetNextFeature()
{
	if(is_start_reading)
	{
		OGRFeature * feature = new OGRFeature(feature_defn.get());

		is_start_reading = false;

		const CDateTime & first_obs = obs->first_obs();
		const CDateTime & last_obs = obs->last_obs();

		feature->SetFID(0);
		feature->SetGeometry(& approx_position);
		feature->SetGeomField(position_field_ind, & position);
		feature->SetField("MarkerName", obs->marker_name().c_str());
		feature->SetField("AntennaModel", obs->antenna_model().c_str());
		feature->SetField("AntennaType", obs->antenna_type().c_str());
		feature->SetField("RecieverModel", obs->reciever_model().c_str());
		feature->SetField("RecieverType", obs->reciever_type().c_str());
		feature->SetField("RecieverVersion", obs->reciever_version().c_str());
		feature->SetField("FirstObsDateTime", first_obs.year(), first_obs.month(), first_obs.day(), first_obs.hour(), first_obs.minute(), first_obs.second());
		feature->SetField("LastObsDateTime", last_obs.year(), last_obs.month(), last_obs.day(), last_obs.hour(), last_obs.minute(), last_obs.second());
		feature->SetField("FirstObsTimeType", obs->first_obs_type().c_str());
		feature->SetField("LastObsTimeType", obs->last_obs_type().c_str());

		return feature;
	}
	
	return NULL;
}

