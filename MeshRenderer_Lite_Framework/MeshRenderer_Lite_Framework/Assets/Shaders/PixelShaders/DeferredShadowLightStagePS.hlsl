#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"
//#include "LightingInclude.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		return diffuseRT.Sample(textureSamplerWrap, pixel.uv);
	//float4 ksAndNs = specularAndNsRT.Sample(textureSamplerWrap, pixel.uv);

	//CaculateBRDFLighting(pixel.worldPos.xyz, pixel.normal, cameraPosition.xyz, 
	//	diffuseRT.Sample(textureSamplerWrap, pixel.uv).xyz,
	//	float4(ksAndNs.xyz, 1.0f), light );
	//float4 result = diffuseRT.Sample(textureSamplerWrap, pixel.uv).xyz;
	//
	//return result;
}