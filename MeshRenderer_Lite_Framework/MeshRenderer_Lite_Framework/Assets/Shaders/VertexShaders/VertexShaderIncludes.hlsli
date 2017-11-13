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

static const unsigned int s_maxBoneCount = 150;
//--------------------------------------------------------------------------------------
// Animation bones buffer per mesh entry
//--------------------------------------------------------------------------------------
cbuffer AnimationBonesBuffer : register(b2)
{
	matrix boneMatrices[s_maxBoneCount];
	float4 bonePositions[s_maxBoneCount];
};