#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"
#include "LightingInclude.hlsli"

struct SimpleLightResult
{
	float4 lightColor : SV_Target0;
	float4 bloomBrightMap : SV_Target1;
};

SimpleLightResult main(PixelInputType pixel)
{
	SimpleLightResult result = (SimpleLightResult)0;
	result.bloomBrightMap.w = 1.0f;

	uint textWidth, textHeight;
	positionRT.GetDimensions(textWidth, textHeight);

	float2 uv = float2(pixel.position.x/float(textWidth), pixel.position.y / float(textHeight));
	
	float4 kd = diffuseRT.Sample(textureSamplerWrap, uv);
	
	//Alpha < 0.f means that the pixel belongs to the skybox, no light should be calculated here
	if (kd.w < 0.f)
	{
		result.lightColor = float4(0, 0, 0, 0);
	}
	else 
	{
		const float3 position = positionRT.Sample(textureSamplerWrap, uv).xyz;

		const float deltaX = sceneLightNoShadow.m_position.x - position.x;
		const float deltaY = sceneLightNoShadow.m_position.y - position.y;
		const float deltaZ = sceneLightNoShadow.m_position.z - position.z;

		const float distSquared = (deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ);
		const float rangeSquared = sceneLightNoShadow.m_range * sceneLightNoShadow.m_range;

		if (distSquared < rangeSquared)
		{
			float3 normal = normalsRT.Sample(textureSamplerWrap, uv).xyz;
			float4 ksAndNs = specularAndNsRT.Sample(textureSamplerWrap, uv);

			float att = 1.0f - (distSquared / rangeSquared);

			float3 lightVec = (sceneLightNoShadow.m_position - position);
			const float lightVecLength = length(lightVec);

			lightVec /= lightVecLength;

			const float3 viewVec = normalize(cameraPosition.xyz - position);

			float attenuation = CalculateAttenuation(1, lightVecLength, sceneLightNoShadow.attConst,
				sceneLightNoShadow.attLinear, sceneLightNoShadow.attQuadratic);

			result.lightColor =
				(att * attenuation *
					CaculateBRDFLighting(normal, kd, float4(ksAndNs.xyz, 1.0f), ksAndNs.w, viewVec,
						sceneLightNoShadow.m_position, float4(sceneLightNoShadow.m_Iambient, 1.0f), 
						float4(sceneLightNoShadow.m_Idiffuse, 1.0f))
				);
		}
		else
		{
			result.lightColor = float4(0, 0, 0, 1.0f);
		}
	}

	float brightness = dot(result.lightColor.xyz, float3(0.2126, 0.7152, 0.0722));

	if(brightness > 1.0)
		result.bloomBrightMap = float4(result.lightColor.xyz, 1.0f);

	result.lightColor = saturate(result.lightColor);

	return result;
}