
#include "data_source.hpp"

CDataSource::CDataSource()
{
	;
}

CDataSource::~CDataSource()
{
	;
}

int CDataSource::open(const string fname)
{
	auto p_layer = new CLayer(fname);

	layer.reset(p_layer);

	return TRUE;
}

OGRLayer * CDataSource::GetLayer(int ind)
{
	if(ind != 0)
		return NULL;

	return layer.get();
}

