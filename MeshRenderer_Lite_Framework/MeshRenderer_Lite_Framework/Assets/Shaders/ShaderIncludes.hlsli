//--------------------------------------------------------------------------------------
// Constant Static variables
//--------------------------------------------------------------------------------------
static const float PI = 3.14159265f;
static const float TWO_PI = 6.2831853f;
static const float EPSILION = 0.00001f;

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

//--------------------------------------------------------------------------------------
// Per Object - Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix worldMtx;
	float4 objectColor;

	int useTexture;
	int useNormalMap;
	int isSkydome;
	int isAnimated;
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