
#include "all.hpp"
#include "rinex.hpp"
#include "data_source.hpp"
#include "layer.hpp"

static int rinex_driver_identify(GDALOpenInfo * info)
{
	if(info->eAccess != GA_ReadOnly)
		return FALSE;

	// TODO - really 17o? 
	if(! EQUAL(CPLGetExtension(info->pszFilename), "17o"))
		return FALSE;

	return TRUE;
}

static GDALDataset * rinex_driver_open(GDALOpenInfo * info)
{
	if(! rinex_driver_identify(info))
		return NULL;
	
	CDataSource * ds = new CDataSource;

    if(! ds->open(info->pszFilename))
    {
        delete ds;

        return NULL;
    }

    return ds;
}

void RegisterOGRRinex()
{
	if(GDALGetDriverByName("RINEX") != NULL)
		return;

    GDALDriver * driver = new GDALDriver();

    driver->SetDescription("RINEX");
    driver->SetMetadataItem(GDAL_DCAP_VECTOR, "YES");
    driver->SetMetadataItem(GDAL_DMD_LONGNAME, "Rinex 2 and 3 driver");
    driver->SetMetadataItem(GDAL_DMD_EXTENSION, "");
    // driver->SetMetadataItem(GDAL_DMD_HELPTOPIC, "drv_rinex.html"); // TODO - create doc
    driver->pfnOpen = rinex_driver_open;
    driver->pfnIdentify = rinex_driver_identify;
    GetGDALDriverManager()->RegisterDriver(driver);
}

