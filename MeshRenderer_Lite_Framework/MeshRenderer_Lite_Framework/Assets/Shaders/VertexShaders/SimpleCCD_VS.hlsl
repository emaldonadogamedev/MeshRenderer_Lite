#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex, in uint vertexID:SV_VertexID)
{
	PixelInputType result = (PixelInputType)0;

	////now proceed with the view and proj. normally
	//result.position = ;
	result.worldPos = float4(ccdData.jointPositions[vertexID].xyz, 1.0f);
	result.position = mul(result.worldPos, viewMtx);
	result.position = mul(result.position, projectionMtx);

	result.color = float4(0, 0, 1.f, 1.f);

	return result;
}