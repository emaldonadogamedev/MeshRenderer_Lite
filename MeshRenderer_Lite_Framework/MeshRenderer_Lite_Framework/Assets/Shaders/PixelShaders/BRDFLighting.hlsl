#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float CalculateAttenuation(int lightType, float d, float C, float L, float Q) {

	return lightType == LT_DIRECTIONAL ? 1.0f :
		min(
			1.0f / (C + (L * d) + (Q * d * d)),
			1.0f
		);
}

float4 Fterm(float4 Ks, float3 L, float3 H) {
	//(1?L?H)^5
	float temp = 1.0f - max(dot(L, H), 0.f);
	temp = temp * temp * temp * temp * temp;

	return Ks + ((float4(1, 1, 1, 1) - Ks) * temp);
}

float Dterm(float3 N, float3 H, float roughness) {
	float term1 = (roughness + 2.0f) / TWO_PI;
	float term2 = pow(max(dot(H, N), 0.f), roughness);

	return term1 * term2;
}

static const float4 Ia = float4(0.1, 0.1, 0.1, 1.0);

float4 CaculateBRDFLighting(float3 vertexPos, float3 vertexNormal, float3 vertexTangent, 
	float3 vertexBiTangent, float2 UVCoords) {
	float4 finalColor = float4(0, 0, 0, 1.0);
	
	const float3 viewVec = normalize(cameraPosition.xyz - vertexPos);
	float3 lightVec;

	float4 diffuse, BRDF;

	for (unsigned int i = 0; i < s_maxLights; ++i)
	{
		if (sceneLights[i].isTaken) {
			if (sceneLights[i].isActive)
			{
				float4 tempColor = float4(0, 0, 0, 1.0);

				lightVec = (sceneLights[i].m_position - vertexPos);
				float lightVecLength = length(lightVec);

				lightVec /= lightVecLength;

				float3 H = normalize(lightVec + viewVec);
				float LdotH = max(dot(lightVec, H), 0.f);

				//KD term
				//Get the diff texture's color
				uint textWidth, textHeight;
				diffTexture.GetDimensions(textWidth, textHeight);
				float4 diffTextureColor;
				if (textWidth > 0 && textHeight > 0)
					diffTextureColor = diffTexture.Sample(textureSamplerWrap, UVCoords);
				else
					diffTextureColor = objectColor;

				diffuse = diffTextureColor / PI;

				float4 F = Fterm(sceneLights[i].m_Ispecular, lightVec, H);
				float D = Dterm(vertexNormal, H, sceneLights[i].roughness);


				BRDF = diffuse + (F * D) / (4.0f * LdotH * LdotH);

				float attenuation = CalculateAttenuation(sceneLights[i].m_lightType, lightVecLength, sceneLights[i].m_ConstantAttenuation,
					sceneLights[i].m_LinearAttenuation, sceneLights[i].m_QuadraticAttenuation);

				tempColor += (
					attenuation *
					((Ia * diffTextureColor) + (sceneLights[i].m_Idiffuse * max(dot(vertexNormal, lightVec), 0.f) * BRDF))
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
	return CaculateBRDFLighting(pixel.worldPos, pixel.normal, pixel.tangent, pixel.bitangent, pixel.uv);
}