#ifndef ALO_GRD_TEST_CELL_H
#define ALO_GRD_TEST_CELL_H

#include <math/BoundingBox.h>
#include <math/pointBox.h>
#include <math/boxBox.h>

namespace alo {

namespace grd {

struct BoxObject {

	BoundingBox _bbox;

	const BoundingBox &aabb() const 
	{ return _bbox; }

	void getAabb(float *b) const
	{ memcpy(b, _bbox.data(), 24); }

	void expandAabb(float *b) const
	{
		b[0] -= .5f;
		b[1] -= .5f;
		b[2] -= .5f;
		b[3] += .5f;
		b[4] += .5f;
		b[5] += .5f;
	}
    
    float mapDistance(const float *p) const 
    {
        if(isPointOutsideBox(p, _bbox.data()))
            return distanceOutsidePointToBox(p, _bbox.data());

        return -distanceInsidePointToBox(p, _bbox.data());
    }
    
    bool intersectBox(const float* b) const
    {
        return boxIntersectBox(b, _bbox.data());
    }

    void mapNormal(float *nml, const float *p) const 
    {
    	normalOnBox(nml, p, _bbox.data());
    }

    void limitStepSize(float &x) const
    {
    	//if(x > 1e-3f && x < .1f) x = .1f;
		//if(x > 2.f) x = 2.f;
    }

};

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

    void calcSpace() {
    	_invTm = _tm;
    	_invTm.inverse();
    }

    const int &objectId() const
    { return _objId; }

    void pointToWorld(float *q) const
    { _tm.transformPoint(q); }

    void pointToLocal(float *q) const
    { _invTm.transformPoint(q); }

};


// T1 is instance type
// T2 is object type
template<typename T1, typename T2>
class ObjectInstancer {

	SimpleBuffer<T1> m_instances;
	ElementVector<T2> m_objs;

public:

	typedef T1 InstanceTyp;

	ObjectInstancer();

	void addObject(T2 *x);

	void createInstances(int n);

	T1 &instance(int i);

	const T1 &c_instance(int i) const;

	const int &numInstances() const;

	void getAabb(float *b, int ii) const;
	float mapDistance(const float *q, 
				const int *inds, const Int2 &range, 
				int &closestObjectId) const;
	float mapDistanceInstance(const float *p, int i) const;
	bool intersectBox(const float* b, int ii) const;
	void mapNormal(Vector3F &nml, const float *p, int ii) const;
	void limitStepSize(float &d, int ii) const;

};

template<typename T1, typename T2>
ObjectInstancer<T1, T2>::ObjectInstancer()
{}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::addObject(T2 *x)
{ m_objs.append(x); }

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
/// todo to world scaling
	return shape->mapDistance(q);
}

template<typename T1, typename T2>
bool ObjectInstancer<T1, T2>::intersectBox(const float* b, int ii) const
{
	const T1 &inst = m_instances[ii];
	const T2 *shape = m_objs.element(inst.objectId()); 

	const BoundingBox wb(b);

	BoundingBox lb;
	for(int i=0;i<8;++i) {
		Vector3F v = wb.corner(i);
		inst.pointToLocal((float *)&v);
		lb.expandBy(v);
	}

	return shape->intersectBox((const float *)&lb);
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::mapNormal(Vector3F &nml, const float *p, int i) const
{
	const T1 &inst = m_instances[i];
	const T2 *shape = m_objs.element(inst.objectId()); 

	float q[3];
	memcpy(q, p, 12);
	inst.pointToLocal(q);
/// todo to world scaling
	shape->mapNormal((float *)&nml, q);
}

template<typename T1, typename T2>
void ObjectInstancer<T1, T2>::limitStepSize(float &d, int ii) const
{
//	const T1 &inst = m_instances[ii];
//	const T2 *shape = m_objs.element(inst.objectId()); 
/// todo scaling distance
//	shape->limitStepSize(d);
}

}

}

#endif