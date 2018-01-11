
#include "test/all.hpp"

int main(const int argc, const char * argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "./test_1 RINEX_FNAME\n");

		return -1;
	}

	const string fname = argv[1];
	GDALDataset * ds;
	OGRLayer * layer;
	OGRFeature * feature;

	GDALAllRegister();
	RegisterOGRRinex();

	if((ds = (GDALDataset *) GDALOpenEx(fname.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL)) == NULL)
	{
		fprintf(stderr, "Open failed\n");

		return -1;
	}

	if((layer = ds->GetLayer(0)) == NULL)
	{
		fprintf(stderr, "Bad layer name or index\n");

		return -1;
	}

	layer->ResetReading();

	while((feature = layer->GetNextFeature()) != NULL)
	{
		printf("\nField num = %u:\n\n\
\tMarker name = %s\n\
\n",
			feature->GetFieldCount(),
			feature->GetFieldAsString("MarkerName"));

		OGRFeature::DestroyFeature(feature);
	}

	GDALClose(ds);
}

