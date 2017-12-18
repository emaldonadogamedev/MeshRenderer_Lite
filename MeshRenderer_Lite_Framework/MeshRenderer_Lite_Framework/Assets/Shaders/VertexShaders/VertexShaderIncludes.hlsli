//--------------------------------------------------------------------------------------
// Per Object - Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix worldMtx;
	float4 color;

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

//--------------------------------------------------------------------------------------
// Curve path const buffer
//--------------------------------------------------------------------------------------
static const unsigned int s_maxControlPointCount = 20;

cbuffer CurvePath_ConstBuffer : register(b3)
{
	float4 controlPointPositions[s_maxControlPointCount];

	float scale;
	float3 padding;
}

//--------------------------------------------------------------------------------------
// Simple Cloth Component const buffer
//--------------------------------------------------------------------------------------
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
// Simple CCD const buffer
//--------------------------------------------------------------------------------------
static const unsigned int s_simpleJointAmount = 5;
struct SimpleCCD_ConstBuffer
{
	float4 jointPositions[s_simpleJointAmount];
};

cbuffer SimpleCCD_ConstBuffer : register(b5)
{
	SimpleCCD_ConstBuffer ccdData;
}