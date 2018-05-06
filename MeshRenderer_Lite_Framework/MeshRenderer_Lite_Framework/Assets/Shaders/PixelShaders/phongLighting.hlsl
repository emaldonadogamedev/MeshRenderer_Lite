#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float CalculateAttenuation(int lightType, float d, float C, float L, float Q) {
	if (lightType == LT_DIRECTIONAL) {
		return 1.0f;
	}

	return min(
		1.0f/(C + (L * d) + (Q * d * d)),
		1.0f
	);
}

float4 CaculatePhongLighting(float3 vertexPos, float3 vertexNormal, float3 vertexTangent, 
	float3 vertexBiTangent, float2 UVCoords, float4 objColor) {
	float4 finalColor = (float4)0;
	
	const float3 viewVec = normalize(cameraPosition.xyz - vertexPos);
	float3 lightVec;

	float4 diffuse, specular;

	for (unsigned int i = 0; i < s_maxLights; ++i)
	{
		if (sceneLights[i].isTaken) {
			float4 tempColor = (float4)0;
			if (sceneLights[i].isActive)
			{
				lightVec = (sceneLights[i].m_position - vertexPos);
				float lightVecLength = length(lightVec);
				
				lightVec /= lightVecLength;

				//KD term
				//Get the diff texture's color
				uint textWidth, textHeight;
				diffTexture.GetDimensions(textWidth, textHeight);
				float4 diffTextureColor;
				if (textWidth > 0 && textHeight > 0)
					diffTextureColor = diffTexture.Sample(textureSamplerWrap, UVCoords);
				else
					diffTextureColor = objColor;

				diffuse = (sceneLights[i].m_Idiffuse * diffTextureColor) * max(dot(vertexNormal, lightVec), 0.f);

				//KS term
				float3 H = normalize(lightVec + viewVec);
				const float NdotH = max(dot(vertexNormal, H), 0.f);

				specular = sceneLights[i].m_Ispecular * pow(NdotH, sceneLights[i].roughness);

				float attenuation = CalculateAttenuation(sceneLights[i].m_lightType, lightVecLength, sceneLights[i].m_ConstantAttenuation,
					sceneLights[i].m_LinearAttenuation, sceneLights[i].m_QuadraticAttenuation);

				tempColor += (
					attenuation *
					(diffuse + specular)
				);

				finalColor += tempColor;
			}
		}
		else
			break;
	}

	return finalColor;
}

float4 main(PixelInputType pixel) : SV_TARGET
{
	return CaculatePhongLighting(pixel.worldPos, pixel.normal, pixel.tangent, pixel.bitangent, pixel.uv, pixel.color);
}