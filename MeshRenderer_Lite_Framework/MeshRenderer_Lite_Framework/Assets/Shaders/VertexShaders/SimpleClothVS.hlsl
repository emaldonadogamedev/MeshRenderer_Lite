#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex, in uint vertexID:SV_VertexID)
{
	PixelInputType result = (PixelInputType)0;

	////now proceed with the view and proj. normally
	//result.position = ;
	result.worldPos = mul(particleData[vertexID].position, worldMtx);
	result.position = mul(result.worldPos, viewMtx);
	result.position = mul(result.position, projectionMtx);

	result.color = float4(1.0f, 0.f, 0.f, 1.f);

	return result;
}