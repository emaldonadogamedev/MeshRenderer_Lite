#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//stl includes
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <thread>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

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

#ifndef SafeRelease
#define SafeRelease(x) if(x) {x->Release(); x = 0;}
#endif

#ifndef SafeDelete
#define SafeDelete(x) if(x) { delete x; x = 0; }
#endif

#ifndef SafeDeleteArr
#define SafeDeleteArr(x) if(x){ delete [] x; x = 0; }
#endif