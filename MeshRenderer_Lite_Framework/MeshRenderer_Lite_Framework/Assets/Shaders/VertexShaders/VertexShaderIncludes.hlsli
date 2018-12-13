//--------------------------------------------------------------------------------------
// Vertex Shader specific constant data
//--------------------------------------------------------------------------------------
static const unsigned int s_maxBoneCount = 120;
static const unsigned int s_maxBoneLocCount = 120 * 6;

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
struct LightViewProj
{
	float4x4 lightViewMtx;
    float4x4 invLightViewMtx;
	float4x4 lightProjectionMtx;
};

cbuffer LightViewProjBuffer : register(b5)
{
		LightViewProj lightViewProj[15];
};

cbuffer LightVolumeProperties : register(b10)
{
    float3 volumeLightPos;
    float volumeLightfarPlane;

    float3 volumeLightUpVector;
    float volumeLightFOV;

    float3 volumeLightForwardVector;
    float volumeLightShadowMapWidth;

    float3 volumeLightRightVector;
    float volumeLightShadowMapHeight;
};