#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float CalculateAttenuation(int lightType, float d, float C, float L, float Q) {

	return lightType == LT_DIRECTIONAL ? 1.0f : 
		min(
			1.0f / (C + (L * d) + (Q * d * d)),
			1.0f
		);
}

float CalculateSpotlightEffect(int lightType, in float3 lightVec, in float3 lightSpotDir, in float outerAngle, 
		in float innerAngle, in float ns)
{
		if (lightType == LT_POINT)
		{
				return 1.0f;
		}
		const float D = normalize(lightSpotDir);
		const float LdotD = dot(lightVec, D);

		if (LdotD < cos(outerAngle))
		{
				return 0.0f;
		}
		else if (LdotD > cos(innerAngle))
		{
				return 1.0f;
		}

		const float spotMinAngle = cos(innerAngle);
		const float spotMaxAngle = cos(outerAngle);
		const float cosAlpha = LdotD;//dot( D, L);

		return pow((cosAlpha - spotMaxAngle) / (spotMinAngle - spotMaxAngle), ns);
}

float4 CaculatePhongLighting(float3 vertexPos, float3 vertexNormal, float3 vertexTangent, 
	float3 vertexBiTangent, float2 UVCoords) 
{
	float4 finalColor = (float4)0;
	
	const float3 viewVec = normalize(cameraPosition.xyz - vertexPos);
	float3 lightVec;

	float4 diffuse, specular;

	for (unsigned int i = 0; i < s_maxLights; ++i)
	{
		if (sceneLights[i].isTaken) {
			if (sceneLights[i].isActive)
			{
				float4 tempColor = (float4)0;

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
					diffTextureColor = objectColor;

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
	return CaculatePhongLighting(pixel.worldPos, pixel.normal, pixel.tangent, pixel.bitangent, pixel.uv);
}