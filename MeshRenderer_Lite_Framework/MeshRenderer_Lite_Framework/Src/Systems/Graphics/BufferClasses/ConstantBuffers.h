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