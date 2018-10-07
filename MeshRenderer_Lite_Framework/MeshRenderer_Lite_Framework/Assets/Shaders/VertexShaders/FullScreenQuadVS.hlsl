#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex, uint vertexID : SV_VERTEXID)
{
		float2 fsqUVs[4] =
		{
				float2(0.0f, 1.0f),
				float2(0.0f, 0.0f),
				float2(1.0f, 0.0f),
				float2(1.0f, 1.0f)
		};

		float4 fqsPositions[4] = //View Space positions
		{
				float4(-1.f, -1.f, 0.f, 1.f),
				float4(-1.f,  1.f, 0.f, 1.f),
				float4( 1.f,  1.f, 0.f, 1.f),
				float4( 1.f, -1.f, 0.f, 1.f)
		};

		PixelInputType result = (PixelInputType)0;

		result.uv = fsqUVs[vertexID];
		result.position = fqsPositions[vertexID];

		return result;
}