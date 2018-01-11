
#ifndef DATA_SOURCE_HPP
#define DATA_SOURCE_HPP

#include "all.hpp"
#include "layer.hpp"

class CDataSource : public GDALDataset
{

	shared_ptr<CLayer> layer;

	public:

		CDataSource();
		~CDataSource();

		int open(const string fname);

		OGRLayer * GetLayer(int ind) override;
		inline int TestCapability(const char * cap) override { return FALSE; }; // TODO - only read in this moment
		inline int GetLayerCount() override { return 1; };

};

#endif

