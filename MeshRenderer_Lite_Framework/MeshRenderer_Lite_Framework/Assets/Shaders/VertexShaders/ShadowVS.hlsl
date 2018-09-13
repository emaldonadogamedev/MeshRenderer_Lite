#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex)
{
		PixelInputType result = (PixelInputType)0;

		float4 pos = float4(vertex.position, 1.0f);

		result.worldPos = mul(pos, worldMtx);
		result.position = mul(result.worldPos, lightViewMtx);
		result.position = mul(result.position, lightProjectionMtx);

		return result;
}