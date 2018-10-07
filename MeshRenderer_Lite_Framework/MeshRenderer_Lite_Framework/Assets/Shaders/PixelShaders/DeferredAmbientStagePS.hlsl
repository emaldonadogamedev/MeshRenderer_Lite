#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	return float4(gGlobalAmbient, 1.f) * diffuseRT.Sample(textureSamplerWrap, pixel.uv);
}