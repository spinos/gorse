/*
 *  TransformComponent.cpp
 *  gorse
 *
 *  2019/4/20
 */

#include "TransformComponent.h"
#include "miscfuncs.h"

namespace alo {

TransformComponent::TransformComponent() :
m_position(0.f, 0.f, 0.f),
m_rotation(0.f, 0.f, 0.f),
m_scale(1.f, 1.f, 1.f),
m_tm(Matrix44F::IdentityMatrix),
m_invTm(Matrix44F::IdentityMatrix)
{}

TransformComponent::~TransformComponent()
{}

Vector3F &TransformComponent::position()
{ return m_position; }

Vector3F &TransformComponent::rotation()
{ return m_rotation; }

Vector3F &TransformComponent::scale()
{ return m_scale; }

void TransformComponent::calculateSpace()
{
	Matrix33F rot = calculateRollPitchYawRotation();
	m_tm = Matrix44F(rot, m_position);
	m_tm.scaleBy(m_scale);
	m_invTm = m_tm;
	m_invTm.inverse();
}

Matrix33F TransformComponent::calculateRollPitchYawRotation() const
{
	if(m_rotation == Vector3F::Zero) return Matrix33F::IdentityMatrix;

	float xang = DegreeToAngle<float>(m_rotation.x);
	float yang = DegreeToAngle<float>(m_rotation.y);
	float zang = DegreeToAngle<float>(m_rotation.z);
/// yaw around y
	Quaternion qyaw(yang, Vector3F::YAxis);
/// pitch around z
	Quaternion qpitch(zang, Vector3F::ZAxis);
/// roll around local x
	Matrix33F rot(qyaw * qpitch);
	Quaternion qroll(xang, rot.row(0));
	rot *= Matrix33F(qroll);

	return rot;
}

void TransformComponent::rayToLocal(float *rayData) const
{
	if(m_scale != Vector3F::One) {

		Vector3F p0(rayData[0], rayData[1], rayData[2]);
/// t1
		Vector3F p1(rayData[0] + rayData[3] * rayData[7], 
					rayData[1] + rayData[4] * rayData[7], 
					rayData[2] + rayData[5] * rayData[7]);

		m_invTm.transformPoint((float *)&p0);
		m_invTm.transformPoint((float *)&p1);

		rayData[7] = p0.distanceTo(p1);
	}

	m_invTm.transformPoint(rayData);
	m_invTm.transformNormal(&rayData[3]);

	if(m_scale != Vector3F::One) {
		Vector3F vn(&rayData[3]);
		vn.normalize();
		vn.extract((char *)&rayData[3]);
	}
}

void TransformComponent::rayToWorld(float *rayData) const
{
	if(m_scale != Vector3F::One) {
		
		Vector3F p0(rayData[0], rayData[1], rayData[2]);
/// t0
		Vector3F p1(rayData[0] + rayData[3] * rayData[6], 
					rayData[1] + rayData[4] * rayData[6], 
					rayData[2] + rayData[5] * rayData[6]);
// t1
		Vector3F p2(rayData[0] + rayData[3] * rayData[7], 
					rayData[1] + rayData[4] * rayData[7], 
					rayData[2] + rayData[5] * rayData[7]);

		m_tm.transformPoint((float *)&p0);
		m_tm.transformPoint((float *)&p1);
		m_tm.transformPoint((float *)&p2);

		rayData[6] = p0.distanceTo(p1);
		rayData[7] = p0.distanceTo(p2);
	}

	m_tm.transformPoint(rayData);
	m_tm.transformNormal(&rayData[3]);
	
	if(m_scale != Vector3F::One) {
		Vector3F vn(&rayData[3]);
		vn.normalize();
		vn.extract((char *)&rayData[3]);
	}
}

void TransformComponent::normalToWorld(float *nml) const
{
	m_tm.transformNormal(nml);
}

void TransformComponent::rayTravel(float *q, const float *rayData) const
{
	q[0] = rayData[0] + rayData[3] * rayData[6]; 
    q[1] = rayData[1] + rayData[4] * rayData[6]; 
    q[2] = rayData[2] + rayData[5] * rayData[6];
}

}
