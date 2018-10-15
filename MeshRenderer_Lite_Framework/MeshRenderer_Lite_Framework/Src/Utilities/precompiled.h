#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//stl includes
#include <algorithm>
#include <assert.h>
#include <future>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <unordered_set>
#include <set>
#include <stack>
#include <string>
#include <time.h>
#include <tuple>
#include <map>
#include <unordered_map>
#include <vector>

//Assimp
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>

static const float aiLengthSquaredBetween2Points_Assimp(const aiVector3D& a, const aiVector3D& b)
{
	const float dx = a.x - b.x;
	const float dy = a.y - b.y;
	const float dz = a.z - b.z;

	return (dx*dx) + (dy*dy) + (dz*dz);
}
static const float aiLengthBetween2Points(const aiVector3D& a, const aiVector3D& b)
{
	sqrt(aiLengthSquaredBetween2Points_Assimp(a, b));
}

//DirectX 11
__pragma(warning(push))
__pragma(warning(disable:4005))
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
__pragma(warning(pop))

#include "dxerr.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//DirectX Texture loading
#include <DirectXTex/DirectXTex.h>

//HRESULT CHECK
#ifndef HR
#ifdef _DEBUG
#define  HR(x) \
{ \
	HRESULT hr = x; \
	if(FAILED(hr)) \
	{ \
		DXTraceW(__FILEW__, __LINE__, hr, L#x, TRUE); \
	} \
}
#else  // !_DEBUG
#define  HR(x) x;
#endif  // _DEBUG
#endif // !HR

//Math
#include <float.h>
#include <math.h>
#include <DirectXMath.h>

static const float XMLengthSquaredBetween2Points(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b)
{
	const float dx = a.m128_f32[0] - b.m128_f32[0];
	const float dy = a.m128_f32[1] - b.m128_f32[1];
	const float dz = a.m128_f32[2] - b.m128_f32[2];

	return (dx*dx) + (dy*dy) + (dz*dz);
}
static const float XMLengthBetween2Points(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b)
{
	sqrtf(XMLengthSquaredBetween2Points(a,b));
}

static float Clamp(const float value, const float minValue = 0.f, const float maxValue = 1.f)
{
		return min(max(minValue, value), maxValue);
}

static float RandFloat(const float minValue = 0.f, const float maxValue = 1.f)
{
		if ((maxValue - minValue) <= 0.0001f)
				return minValue;

		return minValue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxValue - minValue)));
}

static int RandInt(const int minValue = 0, const int maxValue = 100)
{
		if (minValue == maxValue)
				return minValue;

		return minValue + (rand() % (maxValue - minValue)) + 1;
}

static unsigned RandUnsignedInt(const unsigned minValue = 0, const unsigned maxValue = 100)
{
		if (minValue == maxValue)
				return minValue;

		return minValue + (rand() % (maxValue - minValue)) + 1;
}

//Memory delete/release
#ifndef SafeRelease
#define SafeRelease(x) if(x) { x->Release(); x = nullptr;}
#endif

#ifndef SafeDelete
#define SafeDelete(x) if(x) { delete x; x = nullptr; }
#endif

#ifndef SafeDeleteArr
#define SafeDeleteArr(x) if(x){ delete [] x; x = nullptr; }
#endif

//static variables
//directories
static const std::string s_shaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/";
static const std::string s_vertexShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/VertexShaders/";
static const std::string s_pixelShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/PixelShaders/";
static const std::string s_computeShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/ComputeShaders/";
static const std::string s_textureDir = "../MeshRenderer_Lite_Framework/Assets/Textures/";