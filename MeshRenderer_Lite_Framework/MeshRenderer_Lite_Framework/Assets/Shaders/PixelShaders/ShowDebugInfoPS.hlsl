#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	//Don't know 
	float3 valueArr[5] = {
		pixel.position.xyz,
		pixel.normal,
		pixel.tangent,
		pixel.bitangent,
		float3(pixel.uv, 0.f)
	};
	

	//Yep, I'm using the camera position's w coordinate as a debug value index
	const int index = (int)cameraPosition.w;

	return float4(abs(valueArr[index]), 1.0f);
}