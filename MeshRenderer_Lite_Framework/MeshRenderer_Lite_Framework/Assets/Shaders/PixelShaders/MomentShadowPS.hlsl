#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
        const float depth = pixel.position.w / 100.0f;// (pixel.position.z / pixel.position.w) * 10;
		float4 depthValues;
		depthValues.x = depth;            //weird depth
        depthValues.y = pixel.position.z; 
        depthValues.z = pixel.position.w; 
        depthValues.w = pixel.position.z / pixel.position.w;

		return depthValues;
}