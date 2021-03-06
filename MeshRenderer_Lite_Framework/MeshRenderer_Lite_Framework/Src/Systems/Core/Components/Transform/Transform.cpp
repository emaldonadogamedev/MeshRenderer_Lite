#include <Utilities/precompiled.h>


#include <Systems/Core/Components/Transform/Transform.h>
#include<Systems/Core/GameObject/GameObject.h>

Transform::Transform(GameObject* const owner) : IComponent(ComponentType::TRANSFORM, owner)
	, m_position(XMVectorSet(0.f, 0.f, 0.f, 0.f))
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

void Transform::SetScale(const float sx, const float sy, const float sz)
{
		m_scale.m128_f32[0] = sx;
		m_scale.m128_f32[1] = sy;
		m_scale.m128_f32[2] = sz;
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
	auto parentPtr = m_owner->GetParent();
	auto parentTransform = DirectX::XMMatrixIdentity();

	while (parentPtr)
	{
		const auto t = (Transform*)parentPtr->GetComponent(ComponentType::TRANSFORM);
		parentTransform *= t->GetWorldTransform();

		//move the pointer down the chain
		parentPtr = parentPtr->GetParent();
	}

	m_orientationQuat = XMQuaternionRotationMatrix(
		XMMatrixRotationX(m_rotation.m128_f32[0]) *
		XMMatrixRotationY(m_rotation.m128_f32[1]) *
		XMMatrixRotationZ(m_rotation.m128_f32[2])
	);

	m_worldTransform = parentTransform * XMMatrixTranspose (
		XMMatrixRotationX(XM_PIDIV2) *
		XMMatrixScalingFromVector(m_scale) *
		XMMatrixRotationQuaternion(m_orientationQuat) *
		XMMatrixTranslationFromVector(m_position)
	);
}