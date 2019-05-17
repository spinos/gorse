/*
 *  ObjectInstancer.h
 *  gorse
 *
 *  multiple instances of object T2
 *  transform is carried by T1 
 *
 *  2019/5/14
 */

#ifndef ALO_GRD_OBJECT_INSTANCER_H
#define ALO_GRD_OBJECT_INSTANCER_H

#include <math/BoundingBox.h>
#include <math/Matrix44F.h>
#include <math/ElementVector.h>
#include <math/pointBox.h>
#include <math/boxBox.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include "GridSamples.h"

namespace alo {

namespace grd {

struct TestInstance {

    Matrix44F _tm;
    Matrix44F _invTm;
    int _objId;
    
    void setObjectId(int x) {
    	_objId = x;
    }

    void resetSpace() {
    	_tm.setIdentity();
    }

    void setPosition(float x, float y, float z) {
    	_tm.setTranslation(x, y, z);
    }
    
    void setRotation(const Quaternion &q) {
        _tm.setRotation(q);
    }
    
    void setScale(float x) {
        _tm.scaleBy(x);
    }

    void calcSpace() {
    	_invTm = _tm;
    	_invTm.inverse();
    }
    
    void setSpace(const Matrix44F &tm) {
        _tm = tm;
        calcSpace();
    }

    const int &objectId() const
    { return _objId; }

    void pointToWorld(float *q) const
    { _tm.transformPoint(q); }

    void pointToLocal(float *q) const
    { _invTm.transformPoint(q); }
    
    void normalToWorld(float *nml) const
    { _tm.transformNormal(nml); }
    
    void distanceToWorld(float &d) const
    { _tm.transformDistance(d); }

};

template<typename T1, typename T2>
class ObjectInstancer {

	SimpleBuffer<T1> m_instances;
	ElementVector<T2> m_objs;
	ElementVector<PointGridSamplesTyp> m_samps;
    float m_minimumCellSize;
    int m_numInstancedObjects;

public:

	typedef T1 InstanceTyp;
	typedef sds::SpaceFillingVector<PointGridSamplesTyp::SampleTyp> OutSampleTyp;

	ObjectInstancer();
    
    void clearObjects();
    void clear();

	void addObject(T2 *x);

	void createInstances(int n);

	T1 &instance(int i);

	const T1 &c_instance(int i) const;
    
    int numObjects() const;
	const int &numInstances() const;
/// n instanced obj > 0 and n obj >= n instanced obj 
    bool validateNumObjects() const;

	void getAabb(float *b, int ii) const;
	float mapDistance(const float *q, 
				const int *inds, const Int2 &range, 
				int &closestObjectId) const;
	float mapDistanceInstance(const float *p, int i) const;
	void mapNormal(Vector3F &nml, const float *p, int ii) const;
	void limitStepSize(float &d, int ii) const;

	void genInstanceSamples(OutSampleTyp *dest, int ii) const;
    
    void createPhalanx(const int &udim, const int &vdim, 
                    const float &spacing, const float &xzSpan, const float &ySpan, 
                    const float &coverOrigin, const float &scaleSpan,
                    const int &iseed);
                    
    float getMediumObjectSize() const;

    void setMinimumCellSize(float x);
    void setNumInstancedObjects(int x);
/// y no smaller than min_cell_size
    void limitCellSize(float &y) const;

    void verbose() const;
    
};

template<typename T1, typename T2>
ObjectInstancer<T1, T2>::ObjectInstancer() : m_numInstancedObjects(0),
m_minimumCellSize(32.f)
{}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::clearObjects()
{
    m_objs.clear();
    const int n = m_samps.numElements();
    for(int i=0;i<n;++i) {
        delete m_samps.element(i);
    }
    m_samps.clear();
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::clear()
{
    m_instances.purgeBuffer();
    clearObjects();
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::addObject(T2 *x)
{ 
	m_objs.append(x); 
	PointGridSamplesTyp *s = new PointGridSamplesTyp;
	s-> template create<T2>(x);
	m_samps.append(s);
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::createInstances(int n)
{ m_instances.resetBuffer(n); }

template<typename T1, typename T2>
T1 &ObjectInstancer<T1, T2>::instance(int i)
{ return m_instances[i]; }

template<typename T1, typename T2>
const T1 &ObjectInstancer<T1, T2>::c_instance(int i) const
{ return m_instances[i]; }

template<typename T1, typename T2>
const int &ObjectInstancer<T1, T2>::numInstances() const
{ return m_instances.count(); }

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::getAabb(float *b, int ii) const
{ 
	const T1 &inst = m_instances[ii];
	const T2 *shape = m_objs.element(inst.objectId()); 

	shape->getAabb(b);
	shape->expandAabb(b);
	const BoundingBox lb(b);

	BoundingBox wb;
	for(int i=0;i<8;++i) {
		Vector3F v = lb.corner(i);
		inst.pointToWorld((float *)&v);
		wb.expandBy(v);
	}

	memcpy(b, &wb, 24);
}

template<typename T1, typename T2>
float ObjectInstancer<T1, T2>::mapDistance(const float *q, 
				const int *inds, const Int2 &range, 
				int &closestObjectId) const
{
	float md = 1e10f;
	for(int i=range.x;i<range.y;++i) {
        const int &objI = inds[i];

        float d = mapDistanceInstance(q, objI);
        if(md > d) {
            md = d;
            closestObjectId = objI;
        }
    }
    return md;
}

template<typename T1, typename T2>
float ObjectInstancer<T1, T2>::mapDistanceInstance(const float *p, int i) const
{
	const T1 &inst = m_instances[i];
	const T2 *shape = m_objs.element(inst.objectId()); 

	float q[3];
	memcpy(q, p, 12);
	inst.pointToLocal(q);
    
    float d = shape->mapLocalDistance(q);
    
    inst.distanceToWorld(d);
	return d;
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::mapNormal(Vector3F &nml, const float *p, int i) const
{
	const T1 &inst = m_instances[i];
	const T2 *shape = m_objs.element(inst.objectId()); 

	float q[3];
	memcpy(q, p, 12);
	inst.pointToLocal(q);
	nml = shape->mapNormal(q);
    
    inst.normalToWorld((float *)&nml);
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::limitStepSize(float &d, int ii) const
{
//	const T1 &inst = m_instances[ii];
//	const T2 *shape = m_objs.element(inst.objectId()); 
/// todo scaling distance
//	shape->limitStepSize(d);
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::genInstanceSamples(OutSampleTyp *dest, int ii) const
{
	const T1 &inst = m_instances[ii];
	const PointGridSamplesTyp *shapeSamps = m_samps.element(inst.objectId());

	const OutSampleTyp &src = shapeSamps->samples(); 

	PointGridSamplesTyp::SampleTyp ap;
	const int n = src.size();
	for(int i=0;i<n;++i) {
		ap._pos = src[i]._pos;

		inst.pointToWorld((float *)&ap._pos);

		(*dest) << ap;
	}
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::createPhalanx(const int &udim, const int &vdim, 
                    const float &spacing, const float &xzSpan, const float &ySpan, 
                    const float &coverOrigin, const float &scaleSpan,
                    const int &iseed)
{
    createInstances(udim * vdim);

    const int nobj = numObjects();
    
    Uniform<Lehmer> lmlcg(iseed);
    
    for(int j=0;j<vdim;++j) {
        for(int i=0;i<udim;++i) {
            T1 &sample = instance(j*udim + i);

            float rx = coverOrigin + (lmlcg.randf1() - .5f) * xzSpan + spacing * i;
            float ry = (lmlcg.randf1() - .5f) * ySpan;
            float rz = coverOrigin + (lmlcg.randf1() - .5f) * xzSpan + spacing * j;
            float sc = 1.f + (lmlcg.randf1() - .5f) * scaleSpan;
            
            Quaternion roty((lmlcg.randf1() - .5f) * 3.14f, Vector3F::YAxis);

            int objInd = 0;
            if(nobj > 1) objInd = lmlcg.rand(nobj);

            sample.setObjectId(objInd);
            sample.resetSpace();
            sample.setPosition(rx, ry, rz);
            sample.setRotation(roty);
            sample.setScale(sc);
            sample.calcSpace();
        }
    }
}

template<typename T1, typename T2>
int ObjectInstancer<T1, T2>::numObjects() const
{ return m_objs.numElements(); }

template<typename T1, typename T2>
float ObjectInstancer<T1, T2>::getMediumObjectSize() const
{
    float mnD = 1e8f;
    float mxD = -1e8f;
    const int nobj = m_objs.numElements();
    for(int i=0;i<nobj;++i) {
        const T2 *iobj = m_objs.element(i);
        const float *b = iobj->c_aabb();
        float dx = b[3] - b[0];
        float dy = b[4] - b[1];
        float dz = b[5] - b[2];
        if(dx < dy) dx = dy;
        if(dx < dz) dx = dz;
        
        if(mnD > dx) mnD = dx;
        if(mxD < dx) mxD = dx;
    }
    
    return (mnD + mxD) * .5f;
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::verbose() const
{
    std::cout << "\n ObjectInstancer n obj "<< m_objs.numElements()
            << " n instance " << m_instances.count()
            << "\n min cell size " << m_minimumCellSize
            << "\n n instanced obj " << m_numInstancedObjects;
            
    const int nobj = m_objs.numElements();
    for(int i=0;i<nobj;++i) {
        const T2 *iobj = m_objs.element(i);
        const float *b = iobj->c_aabb();
        std::cout << "\n object["<<i<<"] box (("<<b[0]<<","<<b[1]<<","<<b[2]
                                        <<"),("<<b[3]<<","<<b[4]<<","<<b[5]<<"))";
    }
    
    const int n = m_samps.numElements();
    for(int i=0;i<n;++i) {
        const PointGridSamplesTyp *shapeSamps = m_samps.element(i);
        const OutSampleTyp &s = shapeSamps->samples();
        std::cout << "\n object["<<i<<"] n sample " << s.size();
    }
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::setMinimumCellSize(float x)
{ m_minimumCellSize = x; }

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::setNumInstancedObjects(int x)
{ m_numInstancedObjects = x; }

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::limitCellSize(float &y) const
{ if(y < m_minimumCellSize) y = m_minimumCellSize; }

template<typename T1, typename T2>
bool ObjectInstancer<T1, T2>::validateNumObjects() const
{
    if(m_numInstancedObjects < 1) return false;
    const int nobj = m_objs.numElements();
    return nobj >= m_numInstancedObjects;
}

}

}

#endif