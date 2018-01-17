#!/usr/bin/python

import sys, ctypes
from osgeo import ogr

if len(sys.argv) != 2:

    print("2.py OBS_FILE_NAME")

    sys.exit(0)

rinex_drv = ctypes.cdll.LoadLibrary("librinex.so")
rinex_drv.RegisterOGRRinex()

ds = ogr.Open(sys.argv[1])
layer = ds.GetLayerByIndex(0)
spatial_ref = layer.GetSpatialRef()
wkt = spatial_ref.ExportToWkt()

layer.ResetReading()

print("\n", wkt)

while True:

    feature = layer.GetNextFeature()

    if feature is None: break

    print("""
approx position = %s
position = %s
marker name = %s
antenna model = %s
antenna type = %s
reciever model = %s
reciever type = %s
reciever version = %s
first obs = %s (%s)
last obs = %s (%s)
""" %
    (
        feature.GetGeometryRef(),
        feature.GetGeomFieldRef("Position"),
        feature.GetFieldAsString("MarkerName"),
	feature.GetFieldAsString("AntennaModel"),
	feature.GetFieldAsString("AntennaType"),
	feature.GetFieldAsString("RecieverModel"),
	feature.GetFieldAsString("RecieverType"),
	feature.GetFieldAsString("RecieverVersion"),
	feature.GetFieldAsDateTime("FirstObsDateTime"), feature.GetFieldAsString("FirstObsTimeType"),
	feature.GetFieldAsDateTime("LastObsDateTime"), feature.GetFieldAsString("LastObsTimeType")
    ))

