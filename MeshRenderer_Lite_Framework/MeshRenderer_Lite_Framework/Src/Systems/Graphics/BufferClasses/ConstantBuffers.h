#pragma once

#include <DirectXMath.h>

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;
using DirectX::XMMatrixIdentity;
using DirectX::XMVectorSet;

struct ViewProjBuffer
{
	XMMATRIX viewMtx = XMMatrixIdentity();
	XMMATRIX invViewMtx = XMMatrixIdentity();;
	XMMATRIX projectionMtx = XMMatrixIdentity();;
	XMMATRIX invProjectionMtx = XMMatrixIdentity();
	XMMATRIX viewProjection = XMMatrixIdentity();
	XMVECTOR cameraPosition = XMVectorSet(0,0,-17,1.0f);
};

struct PerObectBuffer
{
		XMMATRIX worldMtx;

		int isSkydome;
		int isAnimated;
		int shadowMapPassIdx;
		int padding;
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

enum class GlobalGraphicsDebugType : int
{
		G_DEBUG_NONE,
		G_DEBUG_POSITION,
		G_DEBUG_DEPTH,
		G_DEBUG_UV_COORDS,
		G_DEBUG_NORMALS,
		G_DEBUG_DIFFUSE,
		G_DEBUG_SPECULAR,

		COUNT
};

struct GlobalShaderProperties
{
		XMVECTOR gClearColor = XMVectorSet(0, .3f, 0, 1.0f);

		XMFLOAT3 gGlobalAmbient = XMFLOAT3(.9f, .9f, 0.9f);
		int gDebugInfoType = (int)GlobalGraphicsDebugType::G_DEBUG_NONE;

		int gIsUsingDeferred = 1;
		int gIsUsingIBL;
		float toneMappingExposureControl = 1.f;
		float toneMappingExtraExpControl = 1.f;
};

struct LightVolumeProperties
{
	XMFLOAT3 volumeLightPos;
	float volumeLightfarPlane = 400.f;

	XMFLOAT3 volumeLightUpVector;
	float volumeLightFOV = DirectX::XM_PI / 3.0f;

	XMFLOAT3 volumeLightForwardVector;
	float volumeLightShadowMapWidth = 1024.f;

	XMFLOAT3 volumeLightRightVector;
	float volumeLightShadowMapHeight = 1024.f;

	XMFLOAT4 testAirlightCoefficient = XMFLOAT4(1,1,1,1);
};