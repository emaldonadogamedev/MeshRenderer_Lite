#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	return pixel.color;
}