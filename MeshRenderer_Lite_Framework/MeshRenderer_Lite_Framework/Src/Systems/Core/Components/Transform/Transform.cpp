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
	//m_rotation = DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixRotationX(XM_PIDIV2));
}

Transform::~Transform()
{

}

XMVECTOR& Transform::GetPosition()
{
	return m_position;
}

XMVECTOR& Transform::GetOrientation()
{
	return m_rotation;
}

XMVECTOR& Transform::GetScale()
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

void Transform::SetRotationX(float rot)
{
	m_rotation.m128_f32[0] = rot;
}

void Transform::SetRotationY(float rot)
{
	m_rotation.m128_f32[1] = rot;
}

void Transform::SetRotationZ(float rot)
{
	m_rotation.m128_f32[2] = rot;
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
	m_orientationQuat = XMQuaternionRotationMatrix(XMMatrixRotationRollPitchYawFromVector(m_rotation));

	m_worldTransform = XMMatrixTranspose (
		XMMatrixRotationX(XM_PIDIV2) *
		XMMatrixScalingFromVector(m_scale) *
		XMMatrixRotationQuaternion(m_orientationQuat) *
		XMMatrixTranslationFromVector(m_position)
	);
}