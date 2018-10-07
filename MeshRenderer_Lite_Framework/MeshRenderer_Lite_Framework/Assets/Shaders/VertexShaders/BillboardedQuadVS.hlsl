#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex, uint vertexID : SV_VERTEXID)
{
		float2 quadUVs[4] =
		{
				float2(0.0f, 1.0f),
				float2(0.0f, 0.0f),
				float2(1.0f, 0.0f),
				float2(1.0f, 1.0f)
		};

		float3 viewSpacePositions[4] = 
		{
				float3(-0.5f, -0.5f, 0.f),
				float3(-0.5f,  0.5f, 0.f),
				float3( 0.5f,  0.5f, 0.f),
				float3( 0.5f, -0.5f, 0.f)
		};

		PixelInputType result = (PixelInputType)0;

		uint vertexIdx = vertexID % 4;
		uint pointSpriteIdx = vertexID / 4;

		result.uv = quadUVs[vertexIdx];
		result.worldPos = float4(mul(viewSpacePositions[vertexIdx], (float3x3)invViewMtx) + /*light position here*/, 1.0f);
		result.position = mul(result.worldPos, viewMtx);
		result.position = mul(result.worldPos, projectionMtx);

		return result;
}