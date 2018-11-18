#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	float3 cubeUV = normalize(pixel.worldPos);
	
	float2 uv = SphericalUVMapping(cubeUV);

	float3 iblColor = iblMap2D.Sample(textureSamplerWrap, uv).xyz;
	iblColor *= toneMappingExposureControl;

	float3 result = pow(iblColor / (iblColor + float3(1.f, 1.f, 1.f)), toneMappingExtraExpControl / 2.2);

	//Flag to let the other stages know that no lighting calculation needs to be done here.
	return float4(result, -1.f);
}