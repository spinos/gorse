/*
 *  DrawableResourceArray.h
 *  aloe
 *
 */

#ifndef DRAWABLE_RESOURCE_ARRAY_H
#define DRAWABLE_RESOURCE_ARRAY_H

#include <vector>

namespace alo {

class DrawableResource;

class DrawableResourceArray {

	std::vector<DrawableResource *> m_resources;

public:
	DrawableResourceArray();
	virtual ~DrawableResourceArray();
	
	int numResources() const;

protected:
	bool hasResource(int i=0) const;

	DrawableResource *createResource();
	void setResource(DrawableResource *x, int i=0);
	DrawableResource *resource(int i=0);
	DrawableResource *lastResource();

	void removeLastResource();

private:
	
};

}

#endif
