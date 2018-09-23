#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex, uint vertexID : SV_VERTEXID)
{
		PixelInputType result = (PixelInputType)0;

		result.uv = fsqUVs[vertexID];
		result.position = fqsPositions[vertexID];

		return result;
}