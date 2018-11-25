//--------------------------------------------------------------------------------------
// Constant Static variables
//--------------------------------------------------------------------------------------
static const float PI_DIV4 = 0.7853981625;
static const float PI_DIV2 = 1.570796325;
static const float PI = 3.14159265f;
static const float TWO_PI = 6.2831853f;
static const float EPSILION = 0.0001f;

float det3(float3 a, float3 b, float3 c) // Determinant of a 3x3 passed in as three colum
{
		return a.x*(b.y*c.z - b.z*c.y) + a.y*(b.z*c.x - b.x*c.z) + a.z*(b.x*c.y - b.y*c.x);
}

float2 SphericalUVMapping(float3 N)
{
	return float2(0.5f - (atan2(N.z, -N.x) / TWO_PI), acos(N.y) / PI);
}

float3 UVtoNdirection(float2 uv)
{
	const float TwoPiHalfMinusUx = TWO_PI * (0.5f - uv.x);
	const float sinPIv = sin(PI * uv.y);

	return float3(
		cos(TwoPiHalfMinusUx) * sinPIv,
		sin(TwoPiHalfMinusUx) * sinPIv,
		cos(PI * uv.y)
	);
}

//Constansts for spherical harmonics(used by irradiance mapping in image-based lighting)
const static float basisFunc0 = 0.5f * sqrt(1.f / PI);
const static float halfBy3byPI = 0.5 * sqrt(3.f / PI); // used by basis functions 1 to 3
const static float halfBy15byPI = 0.5f * sqrt(15.f / PI); // used by basis functions 4, 5 & 7
const static float quarterBy5byPI = 0.25f * sqrt(5.f / PI);
const static float quarterBy15byPI = 0.25f * sqrt(15.f / PI);

float SphericalHarmonicsBasisFuncEval(int funcIdx, float x, float y, float z)
{
	switch (funcIdx)
	{
		case 0:
			return basisFunc0;
		break;

		case 1:
			halfBy3byPI * y;
		break;
		
		case 2:
			return halfBy3byPI * z;
		break;

		case 3:
			return halfBy3byPI * x;
		break;

		case 4:
			return halfBy15byPI * x * y;
		break;

		case 5:
			return halfBy15byPI * y * z;
		break;

		case 6:
			return quarterBy5byPI * ((3.f * z * z) - 1.f);
		break;

		case 7:
			return halfBy15byPI * x * z;
		break;

		case 8:
			return quarterBy15byPI * ((x*x) - (y*y));
		break;
	}

	return 0.f;
}

//--------------------------------------------------------------------------------------
// Vertex & Pixel structure definitions
//--------------------------------------------------------------------------------------
struct VertexInputType
{
	float3 position     : POSITION;
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float3 bitangent    : BITANGENT;
	float2 uv           : UV;
	float4 color		: COLOR;

	//Animation bone information
	uint4 boneIDs		: BONES;
	float4 boneWeights	: WEIGHTS;
};

struct PixelInputType
{
	float4 worldPos     : WORLDPOS;
	float4 position     : SV_POSITION;
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float3 bitangent    : BITANGENT;
	float2 uv           : UV;
	float4 color		: COLOR;
};

struct GBufferOutput
{
	float4 Position: SV_Target0;
	float4 Normal: SV_Target1;
	float4 Diffuse: SV_Target2;
	float4 SpecularAndNS: SV_Target3;
};

//--------------------------------------------------------------------------------------
// Per MeshEntry - Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix worldMtx;

	int isSkydome;
	int isAnimated;
	int shadowMapPassIdx;
	int padding_b0;
}

//--------------------------------------------------------------------------------------
// Per Camera - Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ViewProjBuffer : register(b1)
{
	matrix viewMtx;
	matrix invViewMtx;
	matrix projectionMtx;
	matrix invProjectionMtx;
	matrix viewProjection;

	float4 cameraPosition;
};