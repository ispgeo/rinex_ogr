
#include "utils/all.hpp"
#include "utils/base.hpp"

int main(const int argc, const char * argv[])
{
	if(argc != 2)
	{
		perror("./display RINEX_FNAME:");

		return -1;
	}

	const string fname = argv[1];
	char * wkt;
	GDALDataset * ds;
	OGRLayer * layer;
	OGRFeature * feature;
	OGRSpatialReference * spatial_ref;

	GDALAllRegister();
	RegisterOGRRinex();

	check_null((ds = (GDALDataset *) GDALOpenEx(fname.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL)), "Open failed");
	check_null((layer = ds->GetLayer(0)), "Bad layer name or index");
	check_null((spatial_ref = layer->GetSpatialRef()), "Bad spatial reference");
	check(spatial_ref->exportToWkt(& wkt) != OGRERR_NONE, "Unable to convert spatial reference to WKT");

	printf("\nSpatial reference = %s\n", wkt);
	CPLFree(wkt);

	layer->ResetReading();

	auto get_date = [ & ](const string & field_name)
	{
		const int field_ind = feature->GetFieldIndex(field_name.c_str());
		int year, month, day, hour, minute, second, flag;
		stringstream str;

		feature->GetFieldAsDateTime(field_ind, & year, & month, & day, & hour, & minute, & second, & flag);

		str << day << "." << month << "." << year << " " << hour << ":" << minute << ":" << second;

		return str.str();
	};

	while((feature = layer->GetNextFeature()) != NULL)
	{
		OGRPoint * approx_position = (OGRPoint *) feature->GetGeometryRef();
		OGRPoint * position = (OGRPoint *) feature->GetGeomFieldRef("Position");

		printf("Field num = %u:\n\n\
\tApprox position = (%lf, %lf, %lf)\n\
\tPosition = (%lf, %lf, %lf)\n\
\tMarker name = %s\n\
\tAntenna model = %s\n\
\tAntenna type = %s\n\
\tReciever model = %s\n\
\tReciever type = %s\n\
\tReciever version = %s\n\
\tFirst time obs = %s [ %s ]\n\
\tLast time obs = %s [ %s ]\n\
\n",
			feature->GetFieldCount(),
			approx_position->getX(), approx_position->getY(), approx_position->getZ(),
			position->getX(), position->getY(), position->getZ(),
			feature->GetFieldAsString("MarkerName"),
			feature->GetFieldAsString("AntennaModel"),
			feature->GetFieldAsString("AntennaType"),
			feature->GetFieldAsString("RecieverModel"),
			feature->GetFieldAsString("RecieverType"),
			feature->GetFieldAsString("RecieverVersion"),
			get_date("FirstObsDateTime").c_str(), feature->GetFieldAsString("FirstObsTimeType"),
			get_date("LastObsDateTime").c_str(), feature->GetFieldAsString("LastObsTimeType")
		);

		OGRFeature::DestroyFeature(feature);
	}

	GDALClose(ds);
}

