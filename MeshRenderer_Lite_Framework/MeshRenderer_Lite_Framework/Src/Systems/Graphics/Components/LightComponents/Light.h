#pragma once

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

struct Light 
{
		LightType m_lightType = LightType::LT_POINT;
		XMFLOAT3 m_position = XMFLOAT3(0, 0, 0);

		XMVECTOR m_Idiffuse = DirectX::XMVectorSet(1, 1, 1, 1);
		XMVECTOR m_Ispecular = DirectX::XMVectorSet(1, 0, 1, 1);

		XMFLOAT3 m_spotDirection = XMFLOAT3(0, -1, 0);
		float m_spotInnerAngle = 0.2f;

		float m_spotOutterAngle = 0.5f;
		float m_ConstantAttenuation = 0.4f;
		float m_LinearAttenuation = 0.001f;
		float m_QuadraticAttenuation = 0.f;

		float roughness = 5.f;
		int isActive = 1;
		int isTaken = 0;
		int isUsingShadows;
};