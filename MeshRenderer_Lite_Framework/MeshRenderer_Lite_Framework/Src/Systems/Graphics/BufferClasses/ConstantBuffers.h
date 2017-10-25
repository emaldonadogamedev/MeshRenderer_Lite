#pragma once

#include <DirectXMath.h>

using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;

struct ViewProjBuffer
{
	XMMATRIX viewMtx;
	XMMATRIX invViewMtx;
	XMMATRIX projectionMtx;
	XMMATRIX invProjectionMtx;
	XMMATRIX viewProjection;
	XMFLOAT4 cameraPosition;
};

struct PerObectBuffer
{
	XMMATRIX worldMtx;
	XMFLOAT4 color;

	int useTexture;
	int useNormalMap;
	int isSkydome;
	int padding;
};

static const char s_maxBoneCount = 100;
struct AnimationBuffer
{
	XMMATRIX boneMatrices[s_maxBoneCount];
};