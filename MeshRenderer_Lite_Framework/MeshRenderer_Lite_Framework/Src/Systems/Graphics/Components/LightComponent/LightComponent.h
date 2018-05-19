#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

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

struct Light {
	LightType m_lightType = LightType::LT_POINT;
	XMFLOAT3 m_position = XMFLOAT3(0, 0, 0);

	XMVECTOR m_Iambient;
	XMVECTOR m_Idiffuse = DirectX::XMVectorSet(1, 1, 1, 1);
	XMVECTOR m_Ispecular = DirectX::XMVectorSet(1, 1, 1, 1);

	XMFLOAT3 m_spotDirection = XMFLOAT3(0, -1, 0);
	float m_spotInnerAngle = 0.2f;

	float m_spotOutterAngle = 0.5;
	float m_ConstantAttenuation = 1.0f;
	float m_LinearAttenuation = 0.1f;
	float m_QuadraticAttenuation = 0.f;

	float roughness = 10.f;
	int isActive = 1;
	int isTaken = 0;
	int padding;
};

class LightComponent : public IComponent
{
public:
	LightComponent(const GameObject* owner, bool isActive = true, bool useShadows = false);
	virtual ~LightComponent();

	Light* GetLight() const;
	bool IsUsingShadows() const;
	ObjectHandle GetShadowRThandle() const;

	void SetUseShadows(const bool v);
	void SetShadowRThandle(const ObjectHandle& shadowRT);

	static const unsigned int s_maxLights;
	static Light* const GetSceneLightsPtr();

private:
	Light* m_light;
	bool m_useShadows;
	ObjectHandle m_shadowRenderTarget;

	static Light sceneLights[];

	friend class GraphicsSystem;
	friend class GameObject;
};