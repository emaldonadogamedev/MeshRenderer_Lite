#include <Utilities/precompiled.h>

#include <Systems/Core/Components/Transform/Transform.h>

Transform::Transform(GameObject* owner) : IComponent(ComponentType::TRANSFORM, owner)
	, m_position()
	, m_rotation()
	, m_scale(XMVectorSet(1.f, 1.f, 1.f, 0))
	, m_right(XMVectorSet(1.f, 0.f, 0.f, 0))
	, m_up(XMVectorSet(0.f, 1.f, 0.f, 0))
	, m_forward(XMVectorSet(0.f, 0.f, 1.f,0))
	, m_worldTransform()
{

}

Transform::~Transform()
{

}

const XMVECTOR& Transform::GetPosition() const
{
	return m_position;
}

const XMVECTOR& Transform::GetOrientation() const
{
	return m_rotation;
}

const XMVECTOR& Transform::GetScale() const
{
	return m_scale;
}

void Transform::SetPositionn(const XMVECTOR& position)
{
	m_position = position;
}

void Transform::SetScale(const XMVECTOR &scale)
{
	m_scale = scale;
}

void Transform::SetOrientation(const XMVECTOR& orientation)
{
	m_rotation = orientation;
}

void Transform::SetOrientation(const XMMATRIX& orientationMtx)
{
	m_rotation = XMQuaternionRotationMatrix(orientationMtx);
	XMVector3Normalize(m_rotation);
}

const XMVECTOR& Transform::GetRightVector() const
{
	return m_right;
}

const XMVECTOR& Transform::GetUpVector() const
{
	return m_up;
}

const XMVECTOR& Transform::GetForwardVector() const
{
	return m_forward;
}

void Transform::SetRightVector(const XMVECTOR& rightVec)
{
	m_right = rightVec;
}

void Transform::SetUpVector(const XMVECTOR& upVec)
{
	m_up = upVec;
}

void Transform::SetForwardVector(const XMVECTOR& forwardVec)
{
	m_forward = forwardVec;
}

const XMMATRIX& Transform::GetWorldTransform() const
{
	return m_worldTransform;
}

void Transform::UpdateWorldMatrix()
{
	//m_worldTransform = XMMatrixTranspose(
	//	XMMatrixScalingFromVector(m_scale) *
	//	XMMatrixRotationQuaternion(m_rotation) *
	//	XMMatrixTranslationFromVector(m_position)
	//);

	m_worldTransform = XMMatrixTranspose(
		XMMatrixTranslationFromVector(m_position) *
		XMMatrixRotationQuaternion(m_rotation) *
		XMMatrixScalingFromVector(m_scale)
	);
}