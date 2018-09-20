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
		int m_lightType = (int)LightType::LT_POINT;
		XMFLOAT3 m_position = XMFLOAT3(0, 0, 0);

		XMVECTOR m_Iambient = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR m_Idiffuse = DirectX::XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		XMVECTOR m_Ispecular = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		XMFLOAT3 m_spotDirection = XMFLOAT3(0, -1, 0);
		float m_spotInnerAngle = 0.2f;

		float m_spotOutterAngle = 0.5f;
		float m_ConstantAttenuation = 0.4f;
		float m_LinearAttenuation = 0.001f;
		float m_QuadraticAttenuation = 0.f;

		float roughness = 1.0f;
		int isActive = 1;
		int isTaken = 0;
		int isUsingShadows;
};