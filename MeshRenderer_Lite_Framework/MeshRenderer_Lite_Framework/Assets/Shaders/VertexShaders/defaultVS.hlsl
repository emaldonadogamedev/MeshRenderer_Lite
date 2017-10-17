#include "../ShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex)
{
	PixelInputType result = (PixelInputType)0;

	result.worldPos = mul(float4(vertex.position, 1.0f), worldMtx );
	result.position = mul(result.worldPos, viewMtx );
	result.position = mul(result.position, projectionMtx );

	result.color = color;

	return result;
}