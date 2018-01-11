
#include "test/all.hpp"
#include "test/base.hpp"

int main(const int argc, const char * argv[])
{
	if(argc != 2)
	{
		perror("./test_1 RINEX_FNAME:");

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

	while((feature = layer->GetNextFeature()) != NULL)
	{
		OGRPoint * position = (OGRPoint *) feature->GetGeometryRef();
		OGRPoint * approx_position = (OGRPoint *) feature->GetGeomFieldRef("ApproxPosition");

		printf("Field num = %u:\n\n\
\tApprox position = (%lf, %lf, %lf)\n\
\tPosition = (%lf, %lf, %lf)\n\
\tMarker name = %s\n\
\n",
			feature->GetFieldCount(),
			approx_position->getX(), approx_position->getY(), approx_position->getZ(),
			position->getX(), position->getY(), position->getZ(),
			feature->GetFieldAsString("MarkerName")
		);

		OGRFeature::DestroyFeature(feature);
	}

	GDALClose(ds);
}

