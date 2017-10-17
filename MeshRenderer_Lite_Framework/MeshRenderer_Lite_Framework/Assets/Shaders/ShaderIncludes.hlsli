//--------------------------------------------------------------------------------------
// Constant Static variables
//--------------------------------------------------------------------------------------
static const float PI = 3.14159265f;
static const float TWO_PI = 6.2831853f;
static const float EPSILION = 0.001f;

struct VertexInputType
{
	float3 position     : POSITION;
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float3 bitangent    : BITANGENT;
	float2 uv           : UV;
	float4 color		: COLOR;
};

struct PixelInputType
{
	float4 worldPos     : WORLDPOS;
	float4 position     : SV_POSITION;
	float3 normal       : NORMAL;
	float2 uv           : UV;
	float3 tangent      : TANGENT;
	float3 bitangent    : BITANGENT;
	float4 color		: COLOR;
};

//--------------------------------------------------------------------------------------
// Per Object - Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b1)
{
	float4x4 worldMtx;
	float4 color;

	int useTexture;
	int useNormalMap;
	int isSkydome;
	int padding;
}

//--------------------------------------------------------------------------------------
// Per Camera - Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ViewProjBuffer : register(b2)
{
	float4x4 viewMtx;
	float4x4 invViewMtx;
	float4x4 projectionMtx;
	float4x4 invProjectionMtx;
	float4x4 viewProjection;

	float4 cameraPosition;
};