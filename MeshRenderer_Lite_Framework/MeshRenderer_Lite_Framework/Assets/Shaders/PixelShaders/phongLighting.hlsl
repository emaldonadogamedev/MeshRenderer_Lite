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

float calculateFogS(float d)
{
		//1000.f is the far plane
		//0.01f is the near plane
		return (1000.0f - d) / (1000.0f - 0.01f);
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
		if (sceneLights[i].isTaken) 
		{
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
					diffTextureColor = meshMaterial.diffuseKd;

				diffuse = (sceneLights[i].m_Idiffuse * diffTextureColor) * max(dot(vertexNormal, lightVec), 0.f);

				//KS term
				float3 H = normalize(lightVec + viewVec);
				const float NdotH = max(dot(vertexNormal, H), 0.f);

				specular = sceneLights[i].m_Ispecular * float4(meshMaterial.specularKs, 1.0f) * pow(NdotH, sceneLights[i].roughness);

				float attenuation = CalculateAttenuation(sceneLights[i].m_lightType, lightVecLength, sceneLights[i].m_ConstantAttenuation,
					sceneLights[i].m_LinearAttenuation, sceneLights[i].m_QuadraticAttenuation);

				float spotlight = CalculateSpotlightEffect(sceneLights[i].m_lightType, lightVec, sceneLights[i].m_spotDirection,
						sceneLights[i].m_spotOutterAngle, sceneLights[i].m_spotInnerAngle, sceneLights[i].roughness);

				tempColor += (
					attenuation *spotlight *
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
	//return float4(meshMaterial.specularKs, 1.0f);
		float4 lit = CaculatePhongLighting(pixel.worldPos, pixel.normal, pixel.tangent, pixel.bitangent, pixel.uv);

		const float S = calculateFogS(length((pixel.worldPos - cameraPosition.xyz).xyz));

		const float4 tempGlobalAmbient = float4(0.2f, 0.2f, 0.2f, 1.0f);
		const float4 tempFogColor = float4(0.1f, 0.1f, 0.1f, 0.3f);

		const float4 I_local = meshMaterial.emissiveKe + (tempGlobalAmbient * meshMaterial.ambientKa) + lit;

		float4 I_final = (I_local * S) + ((1.0f - S) * tempFogColor);

		return I_final;
}