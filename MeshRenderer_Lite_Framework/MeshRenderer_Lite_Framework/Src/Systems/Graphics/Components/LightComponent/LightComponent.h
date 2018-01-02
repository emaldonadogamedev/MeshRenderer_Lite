#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <DirectXMath.h>

using DirectX::XMVECTOR;

enum class LightType : int
{
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT
};

class LightComponent : public IComponent
{
public:
	LightComponent(const GameObject* owner, bool isActive = true, const LightType lightType = LightType::LT_POINT);
	virtual ~LightComponent();

	const LightType GetLightType()const;
	void SetLightType(const LightType type);

protected:

	XMVECTOR m_position;

	XMVECTOR m_Iambient;
	XMVECTOR m_Idiffuse;
	XMVECTOR m_Ispecular;

	XMVECTOR m_spotDirection;

	LightType m_lightType;

	friend class GraphicsSystem;
	friend class GameObject;
};