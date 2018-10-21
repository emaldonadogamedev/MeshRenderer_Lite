#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		const float wDepth = pixel.position.w;
		float4 depthValues;
		depthValues.x = wDepth;                 //z
		depthValues.y = depthValues.x * wDepth; //z^2
		depthValues.z = depthValues.y * wDepth; //z^3
		depthValues.w = depthValues.z * wDepth; //z^4

		return depthValues;
}