#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	uint textWidth, textHeight;
	diffTexture.GetDimensions(textWidth, textHeight);

	if (textWidth > 0 && textHeight > 0)
	{	
		return diffTexture.Sample(textureSamplerClamp, pixel.uv);
	}
	return pixel.color;
}