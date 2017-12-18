#include "../ShaderIncludes.hlsli"
#include "../VertexShaders/VertexShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	return color;
}