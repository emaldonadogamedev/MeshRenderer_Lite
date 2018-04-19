#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <DirectXMath.h>

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;

enum class LightType : int
{
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT,

	COUNT
};

class LightComponent : public IComponent
{
public:
	LightComponent(const GameObject* owner, bool isActive = true, const LightType lightType = LightType::LT_POINT);
	virtual ~LightComponent();

	const LightType GetLightType()const;
	void SetLightType(const LightType type);

	XMFLOAT3 m_position;
	LightType m_lightType;

	XMVECTOR m_Iambient;
	XMVECTOR m_Idiffuse;
	XMVECTOR m_Ispecular;

	XMVECTOR m_spotDirection;

	float m_spotInnerAngle;
	float m_spotOutterAngle;

	friend class GraphicsSystem;
	friend class GameObject;
};