#pragma once

#include <DirectXMath.h>

using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;
using DirectX::XMMATRIX;
using DirectX::XMVectorSet;
using DirectX::XMMatrixIdentity;

struct ViewProjBuffer
{
	XMMATRIX viewMtx = XMMatrixIdentity();
	XMMATRIX invViewMtx = XMMatrixIdentity();;
	XMMATRIX projectionMtx = XMMatrixIdentity();;
	XMMATRIX invProjectionMtx = XMMatrixIdentity();
	XMMATRIX viewProjection = XMMatrixIdentity();
	XMVECTOR cameraPosition = XMVectorSet(0,0,-17,0);
};

struct PerObectBuffer
{
	XMMATRIX worldMtx;
	XMVECTOR color;

	int useTexture;
	int useNormalMap;
	int isSkydome;
	int isAnimated;
};

static const unsigned char s_maxBoneCount = 100;
struct AnimationBuffer
{
	XMMATRIX boneMatrices[s_maxBoneCount];
	XMVECTOR boneLocations[s_maxBoneCount];
};

static const unsigned int s_maxParticleData = 1500;
struct ClothParticleGPU
{
	XMVECTOR position;
	XMVECTOR normal;
};

struct SimpleCloth_ConstBuffer
{
	ClothParticleGPU particleData[s_maxParticleData];
};

//////////////////////////////////////////////////////////////////////////
//Lights
enum class LightType : int
{
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT,

	COUNT
};

struct Light {
	LightType m_lightType;
	XMFLOAT3 m_position;

	XMVECTOR m_Iambient;
	XMVECTOR m_Idiffuse;
	XMVECTOR m_Ispecular;
	XMVECTOR m_spotDirection;

	float m_spotInnerAngle;
	float m_spotOutterAngle;
	int isActive = 0;
	int isTaken = 0;
};

static const unsigned int s_maxLights = 10;
struct Lights_ConstBuffer
{
	Light sceneLights[s_maxLights];
};