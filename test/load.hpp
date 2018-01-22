
#include "test/all.hpp"
#include "obs/obs.hpp"

string fname;

TEST(CObs, load)
{
	char * wkt;
	GDALDataset * ds;
	OGRLayer * layer;
	OGRFeature * feature;
	OGRSpatialReference * spatial_ref;

	GDALAllRegister();
	RegisterOGRRinex();

	ASSERT_TRUE((ds = (GDALDataset *) GDALOpenEx(fname.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL)) != NULL);
	ASSERT_TRUE((layer = ds->GetLayer(0)) != NULL);
	ASSERT_TRUE((spatial_ref = layer->GetSpatialRef()) != NULL);
	ASSERT_EQ(spatial_ref->exportToWkt(& wkt), OGRERR_NONE);

	ASSERT_STREQ(wkt, "GEOCCS[\"IGS08\",DATUM[\"IGS08\",SPHEROID[\"GRS 1980\",6378137,298.257222101,AUTHORITY[\"EPSG\",\"7019\"]],AUTHORITY[\"EPSG\",\"1141\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],AXIS[\"Geocentric X\",OTHER],AXIS[\"Geocentric Y\",OTHER],AXIS[\"Geocentric Z\",NORTH],AUTHORITY[\"EPSG\",\"6934\"]]");
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

	ASSERT_TRUE((feature = layer->GetNextFeature()) != NULL);

	OGRPoint * approx_position = (OGRPoint *) feature->GetGeometryRef();
	OGRPoint * position = (OGRPoint *) feature->GetGeomFieldRef("Position");

	ASSERT_EQ(feature->GetFieldCount(), 10);
	ASSERT_FLOAT_EQ(approx_position->getX(), 788695.414000);
	ASSERT_FLOAT_EQ(approx_position->getY(), 2859795.335100);
	ASSERT_FLOAT_EQ(approx_position->getZ(), 5627377.825400);
	ASSERT_FLOAT_EQ(position->getX(), 777349.698554);
	ASSERT_FLOAT_EQ(position->getY(), 2950761.698905);
	ASSERT_FLOAT_EQ(position->getZ(), 5860728.263382);
	ASSERT_STREQ(feature->GetFieldAsString("MarkerName"), "log0724a");
	ASSERT_STREQ(feature->GetFieldAsString("AntennaModel"), "-Unknown-");
	ASSERT_STREQ(feature->GetFieldAsString("AntennaType"), "");
	ASSERT_STREQ(feature->GetFieldAsString("RecieverModel"), "014VOACFYDQ1P0WH4C98");
	ASSERT_STREQ(feature->GetFieldAsString("RecieverType"), "JAVAD TRIUMPH1");
	ASSERT_STREQ(feature->GetFieldAsString("RecieverVersion"), "3.5.13 May,18,2016");
	ASSERT_STREQ(get_date("FirstObsDateTime").c_str(), "24.7.2017 9:25:25");
	ASSERT_STREQ(feature->GetFieldAsString("FirstObsTimeType"), "GPS");
	ASSERT_STREQ(get_date("LastObsDateTime").c_str(), "24.7.2017 10:33:52");
	ASSERT_STREQ(feature->GetFieldAsString("LastObsTimeType"), "GPS");
	
	OGRFeature::DestroyFeature(feature);
	GDALClose(ds);
}


