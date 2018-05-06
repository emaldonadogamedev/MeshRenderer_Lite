#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex)
{
	PixelInputType result = (PixelInputType)0;

	float4 pos = float4(vertex.position, 1.0f);

	result.worldPos = mul(pos, worldMtx);
	result.position = mul(result.worldPos, viewMtx);
	result.position = mul(result.position, projectionMtx);
	result.normal = normalize(mul(float4(vertex.normal, 0.f), worldMtx));
	result.tangent = normalize(mul(float4(vertex.tangent, 0.f), worldMtx));
	result.bitangent = normalize(mul(float4(vertex.bitangent, 0.f), worldMtx));
	result.uv = vertex.uv;

	result.color = vertex.color;

	return result;
}