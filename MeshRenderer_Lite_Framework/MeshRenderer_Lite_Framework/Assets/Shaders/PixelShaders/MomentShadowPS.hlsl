#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
        const float depth = pixel.position.w / 100.0f;// (pixel.position.z / pixel.position.w) * 10;
		float4 depthValues;
		depthValues.x = depth;                 //z
		depthValues.y = depthValues.x * depth; //z^2
		depthValues.z = depthValues.y * depth; //z^3
		depthValues.w = depthValues.z * depth; //z^4

		return depthValues;
}