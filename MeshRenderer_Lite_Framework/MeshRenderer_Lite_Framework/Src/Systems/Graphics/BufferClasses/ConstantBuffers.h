#pragma once

#include <DirectXMath.h>

using DirectX::XMVECTOR;
using DirectX::XMMATRIX;

struct ViewProjBuffer
{
	XMMATRIX viewMtx;
	XMMATRIX invViewMtx;
	XMMATRIX projectionMtx;
	XMMATRIX invProjectionMtx;
	XMMATRIX viewProjection;
	XMVECTOR cameraPosition;
};

struct PerObectBuffer
{
	XMMATRIX worldMtx;
	XMVECTOR color;

	int useTexture;
	int useNormalMap;
	int isSkydome;
	int padding;
};

static const unsigned char s_maxBoneCount = 200;
struct AnimationBuffer
{
	XMMATRIX boneMatrices[s_maxBoneCount];
};