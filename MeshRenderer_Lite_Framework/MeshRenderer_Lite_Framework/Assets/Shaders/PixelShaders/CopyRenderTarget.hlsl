#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		return previouslyUsedRT.Sample(textureSamplerWrap, pixel.uv);
}