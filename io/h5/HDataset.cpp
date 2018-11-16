/*
 *  HDataset.cpp
 *  helloHdf
 *
 *  Created by jian zhang on 6/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HDataset.h"
#include <iostream>
namespace alo {

HDataset::HDataset(const std::string & path) : HObject(path)
{}

char HDataset::create(hid_t parentId)
{	
	hid_t createSpace = createFileSpace();
	
	if(createSpace < 0) std::cout<<"\nh data space create failed\n";
	
	hid_t createProps = H5Pcreate(H5P_DATASET_CREATE);
	if(createProps < 0) std::cout<<"\nh create property failed\n";
	
	int ndim = H5Sget_simple_extent_ndims(createSpace);
	
	hsize_t dims[3] = {0, 0, 0};
	hsize_t maxdims[3];
	hsize_t m_chunkSize[3] = {32, 0, 0};
	
	H5Sget_simple_extent_dims(createSpace, dims, maxdims);
	m_chunkSize[0] = dims[0] / 4;
	
	//std::cout<<"d space n dim "<<ndim<<"\n";
	//std::cout<<"d space "<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<" \n";
	//std::cout<<"d chunk "<<m_chunkSize[0]<<" "<<m_chunkSize[1]<<" "<<m_chunkSize[2]<<" \n";
	
	if(H5Pset_chunk(createProps, ndim, m_chunkSize)<0) {
      printf("Error: fail to set chunk\n");
      return -1;
	}
	
	H5Pset_deflate(createProps, 8); 
   	
	fObjectId = H5Dcreate2(parentId, fObjectPath.c_str(), dataType(), createSpace, 
                          H5P_DEFAULT, createProps, H5P_DEFAULT);
		  
	if(fObjectId < 0) {
	    std::cout<<"\nh data set create failed\n";
		return 0;
	}
	
	H5Sclose(createSpace);
	H5Pclose(createProps);
	return 1;
}

char HDataset::open(hid_t parentId)
{
	fObjectId = H5Dopen(parentId, fObjectPath.c_str(), H5P_DEFAULT);
	
	if(fObjectId < 0)
		return 0;
		
	return 1;
}

void HDataset::close()
{
	H5Dclose(fObjectId);
}

int HDataset::objectType() const
{
	return H5G_DATASET;
}

hid_t HDataset::dataType()
{
	return H5T_NATIVE_FLOAT;
}

hid_t HDataset::createFileSpace() const
{
	hsize_t     dims[3];
	dims[0] = (fDimension[0] / 32 + 1) * 32;
	dims[1] = 0;
	dims[2] = 0;
	
	int ndim = 1;
	
	hsize_t maximumDims[3];
	maximumDims[0] = H5S_UNLIMITED;
	maximumDims[1] = 0;
	maximumDims[2] = 0;
		
	return H5Screate_simple(ndim, dims, maximumDims);
}

hid_t HDataset::createMemSpace() const
{
	hsize_t     dims[1];
	dims[0] = fDimension[0];
	
	int ndim = 1;
		
	return H5Screate_simple(ndim, dims, NULL);
}

void HDataset::getSpaceDimension(int * dimensions, int * ndimension) const
{
	hid_t spaceId = H5Dget_space(fObjectId);
	hsize_t dims[3];
	hsize_t maxdims[3];
	H5Sget_simple_extent_dims(spaceId, dims, maxdims);
	*ndimension = H5Sget_simple_extent_ndims(spaceId);
	dimensions[0] = dims[0];
	dimensions[1] = dims[1];
	dimensions[2] = dims[2];
}

char HDataset::hasEnoughSpace() const
{
	int dims[3];
	int ndim;
	getSpaceDimension(dims, &ndim);
	for(int i = 0; i < ndim; i++) {
		if(dims[i] < fDimension[i]) {
			//std::cout<<" data space dim["<<i<<"] = "<<dims[i]<<" not enough for "<<fDimension[i]<<"\n";
			return 0;
		}
	}
	return 1;
}

char HDataset::write(char *data, SelectPart * part)
{
	hid_t dataSpace = H5S_ALL;
	hid_t memSpace = createMemSpace();
	herr_t status;
	if(part) {
		if(part->start[0] == 0)
			resize();
		
		status = H5Sselect_hyperslab(memSpace, H5S_SELECT_SET, part->start, part->stride, part->count, part->block);
		if(status < 0) std::cout<<"failed to select hyperslab\n";
		
		dataSpace = createSpace(part->block);
	}
	else 
		resize();
	
	status = H5Dwrite(fObjectId, dataType(), dataSpace, memSpace, H5P_DEFAULT, data);
	H5Sclose(memSpace);

	if(status < 0)
		return 0;
	
	return 1;
}

char HDataset::read(char *data, SelectPart * part)
{
	if(!hasEnoughSpace()) return 0;
	hid_t dataSpace = H5S_ALL;
	hid_t memSpace = createMemSpace();
	herr_t status;
	
	if(part) {
		status = H5Sselect_hyperslab(memSpace, H5S_SELECT_SET, part->start, part->stride, part->count, part->block);
		if(status < 0) std::cout<<"failed to select hyperslab\n";

		dataSpace = createSpace(part->block);
	}

	status = H5Dread(fObjectId, dataType(), dataSpace, memSpace, H5P_DEFAULT, data);
	H5Sclose(memSpace);
	if(status < 0)
		return 0;	
	return 1;
}

void HDataset::resize()
{
	hsize_t size[1] = {(fDimension[0] / 32 + 1) * 32};
	
	herr_t status = H5Dset_extent(fObjectId, size);
	if(status < 0) std::cout<<"resize failed\n";
	
	int dims[3];
	int ndim;
	getSpaceDimension(dims, &ndim);
	if(dims[0] != size[0])
		std::cout<<"failed to resize to "<<size[0]<<"\n";
}

hid_t HDataset::createSpace(hsize_t * block) const
{
	hsize_t     dims[1];
	dims[0] = block[0];
	
	int ndim = 1;
		
	return H5Screate_simple(ndim, dims, NULL);
}

}
