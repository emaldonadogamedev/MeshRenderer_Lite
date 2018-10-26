#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		const float zDepth = pixel.position.w;//pixel.position.z;
		float4 depthValues;
		depthValues.x = zDepth;                 //z
		depthValues.y = depthValues.x * zDepth; //z^2
		depthValues.z = depthValues.y * zDepth; //z^3
		depthValues.w = depthValues.z * zDepth; //z^4

		return depthValues;
}