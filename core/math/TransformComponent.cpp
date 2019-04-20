/*
 *  TransformComponent.cpp
 *  gorse
 *
 *  2019/4/20
 */

#include "TransformComponent.h"

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
}

}
