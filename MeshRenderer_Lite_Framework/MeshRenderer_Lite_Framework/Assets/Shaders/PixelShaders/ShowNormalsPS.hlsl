#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	return float4(abs(pixel.normal), 1.0f);
}