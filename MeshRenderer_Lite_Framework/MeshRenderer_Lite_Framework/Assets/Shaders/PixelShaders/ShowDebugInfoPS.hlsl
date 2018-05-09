#include "../ShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	//if (cameraPosition.w == 0.f) {
	//	return float4(pixel.position.xyz, 1.0f);
	//}
	//if (cameraPosition.w == 1.f) {
	//	return float4(abs(pixel.normal), 1.0f);
	//}
	//if (cameraPosition.w == 2.f) {
	//	return float4(abs(pixel.tangent), 1.0f);
	//}
	//if (cameraPosition.w == 3.f) {
	//	return float4(abs(pixel.bitangent), 1.0f);
	//}
	//if (cameraPosition.w == 4.f) {
	//	return float4(pixel.uv, 0.f, 1.0f);
	//}

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

	//return float4(cameraPosition.w, 0, 0, 1);
}