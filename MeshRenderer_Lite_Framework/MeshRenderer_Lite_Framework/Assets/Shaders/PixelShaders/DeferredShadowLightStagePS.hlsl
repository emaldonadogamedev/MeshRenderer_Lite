#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"
#include "LightingInclude.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		float2 uv = pixel.uv;

		float3 position = positionRT.Sample(textureSamplerWrap, uv).xyz;
		float3 normal = normalsRT.Sample(textureSamplerWrap, uv).xyz;
		float4 kd = diffuseRT.Sample(textureSamplerWrap, uv);
		float4 ksAndNs = specularAndNsRT.Sample(textureSamplerWrap, uv);

		//float4 ambient = previouslyUsedRT.Sample(textureSamplerWrap, uv);
		
		float4 result = float4(0, 0, 0, 1.0f);

		for (unsigned int i = 0; i < s_maxLights; ++i)
		{
				if (sceneLights[i].isTaken && sceneLights[i].isActive)
				{
						result +=  CaculateBRDFLighting(position, normal, kd, float4(ksAndNs.xyz, 1.0f), ksAndNs.w, cameraPosition.xyz,
								sceneLights[i].m_position, sceneLights[i].m_Iambient, sceneLights[i].m_Idiffuse, sceneLights[i].m_ConstantAttenuation,
								sceneLights[i].m_LinearAttenuation, sceneLights[i].m_QuadraticAttenuation);
				}
				else
						break;
		}

	return result;
}