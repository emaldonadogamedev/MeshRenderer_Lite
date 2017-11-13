#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	uint textWidth, textHeight;
	diffTexture.GetDimensions(textWidth, textHeight);

	if (textWidth > 0 && textHeight > 0)
	{	
		float2 finalUV;
		finalUV.x = pixel.uv.x;
		finalUV.y = pixel.uv.y;

		return diffTexture.Sample(textureSamplerWrap, finalUV);
	}
	return pixel.color;
}