#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <DirectXMath.h>

using namespace DirectX;

class Transform : public IComponent
{
public:
	Transform(GameObject* const owner);
	virtual ~Transform();

	XMVECTOR& GetPosition();
	XMVECTOR& GetOrientation();
	XMVECTOR& GetScale();

	void SetPositionn(const XMVECTOR& position);
	void SetScale(const XMVECTOR &scale);
	void SetScale(const float sx, const float sy, const float sz);
	void SetOrientation(const XMVECTOR& orientation);
	void SetOrientation(const XMMATRIX& orientationMtx);
	void SetRotationX(float rot);
	void SetRotationY(float rot);
	void SetRotationZ(float rot);

	const XMVECTOR& GetRightVector() const;
	const XMVECTOR& GetUpVector() const;
	const XMVECTOR& GetForwardVector() const;

	void SetRightVector(const XMVECTOR& rightVec);
	void SetUpVector(const XMVECTOR& upVec);
	void SetForwardVector(const XMVECTOR& forwardVec);

	const XMMATRIX& GetWorldTransform() const;

	void UpdateWorldMatrix();

protected:
	XMVECTOR m_position;
	XMVECTOR m_rotation;
	XMVECTOR m_orientationQuat;
	XMVECTOR m_scale;

	XMVECTOR m_right;
	XMVECTOR m_up;
	XMVECTOR m_forward;

	XMMATRIX m_worldTransform;

	friend GameObject;
	friend IComponent;
};