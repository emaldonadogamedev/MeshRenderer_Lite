#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	float depthValue = pixel.position.z / pixel.position.w;

	float3 valueArr[5] = {
		pixel.worldPos.xyz,
		pixel.normal,
		pixel.tangent,
		float3(depthValue,depthValue,depthValue),
		float3(pixel.uv, 0.f)
	};
	

	//Yep, I'm using the camera position's w coordinate as a debug value index
	const int index = (int)cameraPosition.w;

	return float4(abs(valueArr[index]), 1.0f);
}