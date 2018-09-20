#pragma once

#include <DirectXMath.h>

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
	XMVECTOR cameraPosition = XMVectorSet(0,0,-17,1.0f);
};

struct PerObectBuffer
{
		XMMATRIX worldMtx;

		int isSkydome;
		int isAnimated;
		int padding[2];
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

struct LightViewProjBuffer
{
		XMMATRIX lightViewMtx;
		XMMATRIX lightProjectionMtx;
};

