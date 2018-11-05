#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"
#include "LightingInclude.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		uint textWidth, textHeight;
		positionRT.GetDimensions(textWidth, textHeight);

		float2 uv = float2(pixel.position.x/float(textWidth), pixel.position.y / float(textHeight));

		float3 position = positionRT.Sample(textureSamplerWrap, uv).xyz;
		
		const float deltaX = sceneLightNoShadow.m_position.x - position.x;
		const float deltaY = sceneLightNoShadow.m_position.y - position.y;
		const float deltaZ = sceneLightNoShadow.m_position.z - position.z;

		const float distSquared = (deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ);
		const float rangeSquared = sceneLightNoShadow.m_range * sceneLightNoShadow.m_range;

		if (distSquared < rangeSquared)
		{
				float3 normal = normalsRT.Sample(textureSamplerWrap, uv).xyz;
				float4 kd = diffuseRT.Sample(textureSamplerWrap, uv);
				float4 ksAndNs = specularAndNsRT.Sample(textureSamplerWrap, uv);

				float att = 1.0f - (distSquared / rangeSquared);

				float3 lightVec = (sceneLightNoShadow.m_position - position);
				const float lightVecLength = length(lightVec);

				lightVec /= lightVecLength;

				const float3 viewVec = normalize(cameraPosition.xyz - position);

				float attenuation = CalculateAttenuation(1, lightVecLength, sceneLightNoShadow.attConst, 
						sceneLightNoShadow.attLinear, sceneLightNoShadow.attQuadratic);

				return 
				saturate(att * attenuation * 
						CaculateBRDFLighting(normal, kd, float4(ksAndNs.xyz, 1.0f), ksAndNs.w, viewVec,
						sceneLightNoShadow.m_position, float4(sceneLightNoShadow.m_Iambient, 1.0f), float4(sceneLightNoShadow.m_Idiffuse, 1.0f))
				);
		}

		return float4(0, 0, 0, 1.0f);
}