#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float CalculateAttenuation(in int lightType, in float d, in float C, in float L, in float Q) {

		return lightType == LT_DIRECTIONAL ? 1.0f :
				min(
						1.0f / (C + (L * d) + (Q * d * d)),
						1.0f
				);
}

float4 Fterm(in float4 Ks, in float3 L, in float3 H) {
		//(1?L?H)^5
		float temp = 1.0f - max(dot(L, H), 0.f);
		temp = temp * temp * temp * temp * temp;

		return Ks + ((float4(1, 1, 1, 1) - Ks) * temp);
}

float Dterm(in float3 N, in float3 H, in float roughness) {
		float term1 = (roughness + 2.0f) / TWO_PI;
		float term2 = pow(max(dot(N, H), 0.f), roughness);

		return term1 * term2;
}

float4 CaculateBRDFLighting(float3 vertexPos, float3 vertexNormal, float3 vertexTangent,
		float3 vertexBiTangent, float2 UVCoords) {
		float4 finalColor = float4(0, 0, 0, 1.0);

		const float3 viewVec = normalize(cameraPosition.xyz - vertexPos);
		const float vDotN = max(dot(viewVec, vertexNormal), 0);
		float3 lightVec;

		float4 diffuse, BRDF;

		for (unsigned int i = 0; i < s_maxLights; ++i)
		{
				if (sceneLights[i].isTaken)
				{
						if (sceneLights[i].isActive)
						{
								//KD term
								//Get the diff texture's color
								uint textWidth, textHeight;
								diffTexture.GetDimensions(textWidth, textHeight);
								float4 objDiffColor;
								if (textWidth > 0 && textHeight > 0)
										objDiffColor = diffTexture.Sample(textureSamplerWrap, UVCoords);
								else
										objDiffColor = meshMaterial.diffuseKd;
								objDiffColor = meshMaterial.diffuseKd;

								//Ambient term
								float4 tempColor = sceneLights[i].m_Iambient * objDiffColor;

								lightVec = (sceneLights[i].m_position - vertexPos);
								float lightVecLength = length(lightVec);

								lightVec /= lightVecLength;

								float LdotN = max(dot(lightVec, vertexNormal), 0.f);

								float3 H = normalize(lightVec + viewVec);
								float LdotH = max(dot(lightVec, H), 0.f);

								diffuse = (objDiffColor) / PI;

								float4 F = Fterm(float4(meshMaterial.specularKs, 1.0f), lightVec, H);
								float D = Dterm(vertexNormal, H, meshMaterial.specularPowerNs);
								float G = 1.0f / (LdotH * LdotH);

								BRDF = diffuse + (F * G * D) / (4.0f * LdotN * vDotN);

								float attenuation = CalculateAttenuation(sceneLights[i].m_lightType, lightVecLength, sceneLights[i].m_ConstantAttenuation,
										sceneLights[i].m_LinearAttenuation, sceneLights[i].m_QuadraticAttenuation);

								tempColor += (
										attenuation *
										((sceneLights[i].m_Idiffuse * max(dot(vertexNormal, lightVec), 0.f) * BRDF))
										);

								finalColor += tempColor;
						}
				}
				else
				{
						break;
				}
		}

		return finalColor;
}

float4 main(PixelInputType pixel) : SV_TARGET
{
	float4 brdfColor = CaculateBRDFLighting(pixel.worldPos, pixel.normal, pixel.tangent, pixel.bitangent, pixel.uv);
	return brdfColor;

	/*


	float bias;
	float2 projectTexCoord;

	// Set the bias value for fixing the floating point precision issues.
	bias = 0.001f;

	for (unsigned int i = 0; i < s_maxLights; ++i)
	{
			if (sceneLights[i].isTaken)
			{
					if (sceneLights[i].isActive)
					{
							mul(sceneLights[i].m_position,

							// Calculate the projected texture coordinates.
							projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
							projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
					}
			}
	}

	return
	*/
}