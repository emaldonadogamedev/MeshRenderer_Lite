#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(in uint vertexID:SV_VertexID)
{
	PixelInputType result = (PixelInputType)0;

	//retrieve both the particle and vertex index
	int bonePosIndex = vertexID / 6;
	int vertexInQuad = vertexID % 6;

	//billboarded positions that begin in view space
	float3 billboardVtxPositions[6] = 
	{
		{-1.0f, 1.0f, 0.f},
		{ 1.0f, 1.0f, 0.f},
		{ 1.0f,-1.0f, 0.f},
		{-1.0f, 1.0f, 0.f},
		{ 1.0f,-1.0f, 0.f},
		{-1.0f,-1.0f, 0.f}
	};

	//Adjust the x,y values of the position(View Space) according to the specific vertex
	float3 position = billboardVtxPositions[vertexInQuad];

	matrix boneTransform = boneMatrices[bonePosIndex];

	//multiply with inverse view matrix to move position into world space
	//also apply the offset of the bone position to move it to the right spot
	//position = mul(position, (float3x3)boneTransform) + bonePositions[bonePosIndex];
	position = mul(position, (float3x3) transpose(invViewMtx));// +bonePositions[bonePosIndex];

	////now proceed with the view and proj. normally
	result.position = float4(position, 1.0);
	result.position = mul(result.position, viewMtx);
	//result.position = mul(result.position, projectionMtx);
	
	result.color = float4(1.f, 0.f, 0.f, 1.f);

	return result;
}