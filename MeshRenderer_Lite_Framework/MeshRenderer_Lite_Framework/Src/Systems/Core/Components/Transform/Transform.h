#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <DirectXMath.h>

using namespace DirectX;

class Transform : public IComponent
{
public:
	Transform(GameObject* owner);
	virtual ~Transform();

	const XMVECTOR& GetPosition() const;
	const XMVECTOR& GetOrientation() const;
	const XMVECTOR& GetScale() const;

	void SetPositionn(const XMVECTOR& position);
	void SetScale(const XMVECTOR &scale);
	void SetOrientation(const XMVECTOR& orientation);
	void SetOrientation(const XMMATRIX& orientationMtx);

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
	XMVECTOR m_scale;

	XMVECTOR m_right;
	XMVECTOR m_up;
	XMVECTOR m_forward;

	XMMATRIX m_worldTransform;

	friend GameObject;
	friend IComponent;
};