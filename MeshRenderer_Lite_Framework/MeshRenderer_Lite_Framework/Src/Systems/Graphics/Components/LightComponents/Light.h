#pragma once

#include <DirectXMath.h>

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;

using DirectX::XMVectorSet;
using DirectX::XMMatrixIdentity;

enum class LightType : int
{
		LT_DIRECTIONAL,
		LT_POINT,
		LT_SPOT,

		COUNT
};

static const unsigned int s_maxSimpleLights = 1500;
struct SimpleLight
{
		XMFLOAT3 m_position;
		float m_range;

		XMFLOAT3 m_Iambient = XMFLOAT3(0.4f, 0.4f, 0.4f);
		int isTaken = 0;

		XMFLOAT3 m_Idiffuse = XMFLOAT3(0.8f, 0.8f, 0.8f);
		int isActive = 0;

		float attConst = 1.f;
		float attLinear = 0.7f;
		float attQuadratic = 0.1f;
		float padding;
};

struct LightViewProj
{
		XMMATRIX lightViewMtx = XMMatrixIdentity();
		XMMATRIX invLightViewMtx = XMMatrixIdentity();
		XMMATRIX lightProjectionMtx = XMMatrixIdentity();
};

static const unsigned int s_maxShadowLights = 15;
struct Light 
{
		int m_lightType = (int)LightType::LT_POINT;
		XMFLOAT3 m_position = XMFLOAT3(0, 0, 0);

		XMFLOAT3 m_Iambient = XMFLOAT3(0.4f, 0.4f, 0.4f);
		float m_spotInnerAngle = 0.2f;

		//Leave diffuse color with an alpha channel for color blending
		XMVECTOR m_Idiffuse = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);

		XMFLOAT3 m_Ispecular = XMFLOAT3(1.0f, 1.0f, 1.0f);
		float m_spotOutterAngle = 0.5f;

		XMFLOAT3 m_spotDirection = XMFLOAT3(0, -1, 0);
		float m_ConstantAttenuation = 0.4f;

		float m_LinearAttenuation = 0.001f;
		float m_QuadraticAttenuation = 0.f;
		float roughness = 1.0f;
		int isActive = 1;

		int isTaken = 0;
		int isUsingShadows = 1;
		int isUsingSoftShadows = 1;
		int padding;
};