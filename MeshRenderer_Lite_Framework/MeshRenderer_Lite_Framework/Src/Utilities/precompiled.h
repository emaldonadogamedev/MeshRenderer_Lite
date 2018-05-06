#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//stl includes
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <unordered_set>
#include <set>
#include <stack>
#include <string>
#include <thread>
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