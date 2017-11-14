#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(in uint vertexID:SV_VertexID)
{
	PixelInputType result = (PixelInputType)0;

	//retrieve both the particle and vertex index
	uint bonePosIndex = vertexID / 4;
	uint vertexInQuad = vertexID % 4;

	//Adjust the x,y values of the position(View Space) according to the specific vertex
	float4 position;// = billboardVtxPositions[vertexInQuad];
	position.x = (vertexInQuad % 2) ? 10.f : -10.0f;
	position.y = (vertexInQuad & 2) ? 10.f : -10.0f;
	position.z = 0.f;
	position.w = 1.0f;

	result.position = position;

	//multiply with inverse view matrix to move position into world space
	//also apply the offset of the bone position to move it to the right spot
	result.position = mul(position, invViewMtx);// - bonePositions[bonePosIndex];

	//now proceed with the view and proj. normally
	result.position = mul(result.position, worldMtx);// <-- not needed, already done on the last line
	result.position = mul(result.position, viewMtx);
	result.position = mul(result.position, projectionMtx);

	result.color = float4(1.f, 0.f, 0.f, 1.f);

	return result;
}