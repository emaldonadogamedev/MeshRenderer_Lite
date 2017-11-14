#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(in uint vertexID:SV_VertexID)
{
	PixelInputType result = (PixelInputType)0;

	//retrieve both the particle and vertex index
	int bonePosIndex = vertexID / 6;
	int vertexInQuad = vertexID % 6;

	//billboarded positions that begin in view space
	float4 billboardVtxPositions[6] = 
	{
		{-0.5f, 0.5f, 0.f, 1.f},
		{ 0.5f, 0.5f, 0.f, 1.f},
		{ 0.5f,-0.5f, 0.f, 1.f},
		{-0.5f, 0.5f, 0.f, 1.f},
		{ 0.5f,-0.5f, 0.f, 1.f},
		{-0.5f,-0.5f, 0.f, 1.f}
	};

	//Adjust the x,y values of the position(View Space) according to the specific vertex
	float4 position = billboardVtxPositions[vertexInQuad];
	position.xy = position.xy * 10.f;

	//multiply with inverse view matrix to move position into world space
	//also apply the offset of the bone position to move it to the right spot
	//position = mul(position, invViewMtx);// +bonePositions[bonePosIndex];

	//now proceed with the view and proj. normally
	//result.worldPos = mul(position, worldMtx);// <-- not needed, already done on the last line
	result.position = mul(result.position, viewMtx);
	result.position = mul(result.position, projectionMtx);

	result.color = float4(1.f, 0.f, 0.f, 1.f);

	return result;
}