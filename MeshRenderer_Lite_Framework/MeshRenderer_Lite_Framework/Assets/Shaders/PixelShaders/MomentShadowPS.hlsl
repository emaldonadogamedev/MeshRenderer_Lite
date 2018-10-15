#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
		const float wDepth = pixel.position.w;
		float4 depthValues;
		depthValues.z = wDepth;                 //z
		depthValues.y = depthValues.x * wDepth; //z^2
		depthValues.z = depthValues.y * wDepth; //z^3
		depthValues.w = depthValues.z * wDepth; //z^4

		return fdepthValues;
}