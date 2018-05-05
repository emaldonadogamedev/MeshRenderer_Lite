#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 CaculatePhongLighting(float3 vertexPos, float3 vertexNormal, float3 vertexTangent, float3 vertexBiTangent, float2 UVCoords) {
	float4 finalColor = (float4)0;
	
	const float3 viewVec = normalize(cameraPosition.xyz - vertexPos);
	float3 lightVec;

	for (unsigned int i = 0; i < s_maxLights; ++i)
	{
		if (sceneLights[i].isTaken) {
			float4 tempColor = (float4)0;
			if (sceneLights[i].isActive)
			{
				lightVec = normalize(sceneLights[i].m_position - vertexPos);

				//KD term
				//Get the diff texture's color
				uint textWidth, textHeight;
				diffTexture.GetDimensions(textWidth, textHeight);
				float4 diffTextureColor;
				if (textWidth > 0 && textHeight > 0)
					diffTextureColor = diffTexture.Sample(textureSamplerWrap, UVCoords);
				else
					diffTextureColor = float4(1,1,1,1);

				tempColor += (sceneLights[i].m_Idiffuse * diffTextureColor) * max(dot(vertexNormal, lightVec), 0.f);

				//KS term
				float3 reflectionVec = normalize( (2.f * dot(viewVec, vertexNormal) * vertexNormal) - viewVec);
				tempColor += sceneLights[i].m_Ispecular * pow(max(dot(reflectionVec, viewVec), 0.f), sceneLights[i].roughness);

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