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

static const unsigned int s_maxControlPointCount = 20;

cbuffer CurvePath_ConstBuffer : register(b3)
{
	float4 controlPointPositions[s_maxControlPointCount];

	float scale;
	float3 padding;
}

cbuffer CCD_TargetConstBuffer : register(b4)
{
	matrix worldMatrix;
}