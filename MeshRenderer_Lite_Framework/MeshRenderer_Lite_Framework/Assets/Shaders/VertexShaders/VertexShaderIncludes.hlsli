//--------------------------------------------------------------------------------------
// Vertex Shader specific constant data
//--------------------------------------------------------------------------------------
static const unsigned int s_maxBoneCount = 120;
static const unsigned int s_maxBoneLocCount = 120 * 6;

const float2 fsqUVs[4] =
{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f)
};
const float4 fqsPositions[4] = //View Space positions
{
		float4(-1.f, -1.f, 0.f, 1.f),
		float4(-1.f,  1.f, 0.f, 1.f),
		float4( 1.f,  1.f, 0.f, 1.f),
		float4( 1.f, -1.f, 0.f, 1.f)
};

//--------------------------------------------------------------------------------------
// Animation bones buffer per mesh entry
//--------------------------------------------------------------------------------------
cbuffer AnimationBonesBuffer : register(b2)
{
	matrix boneMatrices[s_maxBoneCount];

	//Debug
	float4 bonePositions[s_maxBoneLocCount];
};

static const unsigned int s_maxControlPointCount = 20;

cbuffer CurvePath_ConstBuffer : register(b3)
{
	float4 controlPointPositions[s_maxControlPointCount];

	float scale;
	float3 padding_b3;
}

struct ClothParticleGPU
{
	float4 position;
	float4 normal;
};
static const unsigned int s_maxParticleData = 1500;

cbuffer SimpleCloth_ConstBuffer : register(b4)
{
	ClothParticleGPU particleData[s_maxParticleData];
}

//--------------------------------------------------------------------------------------
// Per Light - Shadow Map variables
//--------------------------------------------------------------------------------------
cbuffer LightViewProjBuffer : register(b5)
{
		matrix lightViewMtx;
		matrix lightProjectionMtx;
};