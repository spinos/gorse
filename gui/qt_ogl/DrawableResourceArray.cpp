/*
 *  DrawableResourceArray.cpp
 *  aloe
 *
 */

#include "DrawableResourceArray.h"
#include "DrawableResource.h"

namespace alo {

DrawableResourceArray::DrawableResourceArray()
{}

DrawableResourceArray::~DrawableResourceArray()
{
	std::vector<DrawableResource *>::iterator it = m_resources.begin();
	for(;it!=m_resources.end();++it) {
		if(*it) delete *it;
	}
}

bool DrawableResourceArray::hasResource(int i) const
{
	if(m_resources.size() < i+1) return false;
	return m_resources[i] != 0;
}

DrawableResource *DrawableResourceArray::createResource()
{ return new DrawableResource; }

void DrawableResourceArray::setResource(DrawableResource *x, int i)
{
	if(m_resources.size() < i+1)
		m_resources.resize(i+1, 0);
	m_resources[i] = x; 
}

DrawableResource *DrawableResourceArray::resource(int i)
{ return m_resources[i]; }

}
