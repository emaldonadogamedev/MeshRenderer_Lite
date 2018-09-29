#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
		float depthValue = abs(pixel.position.z / pixel.position.w);

		return float4(depthValue, depthValue, depthValue, 1.0f);
}