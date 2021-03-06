#include "../ShaderIncludes.hlsli"
#include "../VertexShaders/VertexShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"
#include "LightingInclude.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	float2 uv = pixel.uv;
	
	float4 kd = diffuseRT.Sample(textureSamplerWrap, uv);
	//Alpha < 0.f means that the pixel belongs to the skybox, no light should be calculated here
	if (kd.w < 0.f)
		return float4(0, 0, 0, 0);

	float3 position = positionRT.Sample(textureSamplerWrap, uv).xyz;
	float3 normal = normalsRT.Sample(textureSamplerWrap, uv).xyz;
	float4 ksAndNs = specularAndNsRT.Sample(textureSamplerWrap, uv);
	
	float4 result = float4(0, 0, 0, 1.0f);

	for (unsigned int i = 0; i < s_maxLights; ++i)
	{
		if (sceneLights[i].isTaken && sceneLights[i].isActive)
		{
			float lightIntensity = 1.0f;
			if (sceneLights[i].isUsingShadows)
			{
				// Set the bias value for fixing the floating point precision issues.
				float bias = 0.001f;

				//get pos into light's point of view
				float4 lightViewPosition = float4(position, 1.0f); // world space pos.
				lightViewPosition = mul(lightViewPosition, lightViewProj[i].lightViewMtx);
				lightViewPosition = mul(lightViewPosition, lightViewProj[i].lightProjectionMtx);

				if (!sceneLights[i].isUsingSoftShadows)
				{
					// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
					if (lightViewPosition.w > 0)
					{
						// Calculate the projected texture coordinates.
						float2 projectTexCoord;
						projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
						projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;

						if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
						{
							// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
							uint shadowMapW, shadowMapH;
							shadowMaps[i].GetDimensions(shadowMapW, shadowMapH);

							float depthValue = shadowMaps[i][uint2(projectTexCoord.x * shadowMapW, projectTexCoord.y * shadowMapH)].x;

							// Calculate the depth of the light.
                            float lightDepthValue = lightViewPosition.w / 100.0f;// (lightViewPosition.z / lightViewPosition.w) * 10;

							// Subtract the bias from the lightDepthValue.
							lightDepthValue -= bias;

							// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this 
							//pixel. If the light is in front of the object then light the pixel, if not then shadow this pixel since an object 
							//(occluder) is casting a shadow on it.
							if (lightDepthValue > depthValue)
							{
								lightIntensity = 0.f;
								//lightIntensity = saturate(dot(normal, normalize(sceneLights[i].m_position - position)));
								//kd = saturate(kd * lightIntensity);
								//kd.w = 1.0f;
								// Calculate the amount of light on this pixel.
								//lightIntensity = saturate(dot(normal, normalize(sceneLights[i].m_position - position) ) );
								//
								//if (lightIntensity > 0.0f)
								//{
								//		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
								//		//kd += (kd * lightIntensity);
								//
								//		///Saturate the final light color.
								//		kd = saturate(kd * lightIntensity);
								//}
							}
						}
					}
				}

				//Soft shadows!
				else 
				{
					// Calculate the projected texture coordinates.
					float2 projectTexCoord;
					projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
					projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;

					if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
					{
						// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
						uint shadowMapW, shadowMapH;
						shadowMaps[i].GetDimensions(shadowMapW, shadowMapH);

						const float alpha = 0.001f;
						float4 blurredDepthValue = shadowMaps[i][uint2(projectTexCoord.x * shadowMapW, projectTexCoord.y * shadowMapH)];
                        blurredDepthValue.y = blurredDepthValue.x * blurredDepthValue.x;
                        blurredDepthValue.z = blurredDepthValue.y * blurredDepthValue.x;
                        blurredDepthValue.w = blurredDepthValue.z * blurredDepthValue.x;

						float4 bPrime = (1.0f - alpha) * blurredDepthValue;
						bPrime += alpha * float4(0.5f, 0.5f, 0.5f, 0.5f);

						const float3 A = float3(1.0f, bPrime.x, bPrime.y);
						const float3 B = float3(bPrime.x, bPrime.y, bPrime.z);
						const float3 C = float3(bPrime.y, bPrime.z, bPrime.w);

						// Calculate the depth of the light.
						float zf = lightViewPosition.w / 100.0f;
						//float zf = positionRT.Sample(textureSamplerWrap, uv).w;

						// Subtract the bias from the lightDepthValue.
						zf -= bias;

						const float3 Z = float3(1.0f, zf, zf * zf);

						const float d = det3(A, B, C);
						const float c1 = det3(Z, B, C) / d;
						const float c2 = det3(A, Z, C) / d;
						const float c3 = det3(A, B, Z) / d;
						
						//Solve using quadratic formula
						const float denominator = 2.0f * c3;
						const float bSquared = c2 * c2;
						const float fourAC = 4.0f * c3 * c1;
						const float sqrRootResult = sqrt(bSquared - fourAC);

						//Variables to solve the quadratic equation
						float z2 = ((-c2) - sqrRootResult) / denominator;
						float z3 = ((-c2) + sqrRootResult) / denominator;

						//make sure z2 is less than z3
						if (z2 > z3)
						{
							const float temp = z2;
							z2 = z3;
							z3 = temp;
						}

						//The returned value G is reversed in sense from the usual s value
						float G;

						if (zf <= z2)
						{
							G = 0.f;
						}
						else if (zf <= z3)
						{
							const float num = (zf * z3) - (bPrime.x * (zf + z3)) + bPrime.y;
							const float den = (z3 - z2) * (zf - z2);
							G = num / den;
						}
						else
						{
							const float num = (z2 * z3) - (bPrime.x * (z2 + z3)) + bPrime.y;
							const float den = (zf - z2) * (zf - z3);
							G = 1.0f - (num / den);
						}

						lightIntensity = saturate(1.0f - G);
					}
				}
			}// END - if (sceneLights[i].isUsingShadows)

            float3 lightVec;
            float lightVecLength = 1.f;

            if (sceneLights[i].m_lightType == LT_DIRECTIONAL)
            {
                lightVec = normalize(-sceneLights[i].m_spotDirection);
                lightVecLength = 1.0f;
            }
            else
            {
                lightVec = (sceneLights[i].m_position - position);
                lightVecLength = length(lightVec);
                lightVec /= lightVecLength;
            }

			float attenuation = CalculateAttenuation(sceneLights[i].m_lightType, lightVecLength, sceneLights[i].m_ConstantAttenuation,
					sceneLights[i].m_LinearAttenuation, sceneLights[i].m_QuadraticAttenuation);

			float spotlight = CalculateSpotlightEffect(sceneLights[i].m_lightType, lightVec, sceneLights[i].m_spotDirection,
					sceneLights[i].m_spotOutterAngle, sceneLights[i].m_spotInnerAngle, sceneLights[i].roughness);

			const float3 viewVec = normalize(cameraPosition.xyz - position);

			result +=
			saturate(
					attenuation * spotlight *

					CaculateBRDFLighting(normal, lightIntensity * kd, lightIntensity * float4(ksAndNs.xyz, 1.0f), ksAndNs.w, viewVec,
							lightVec, float4(sceneLights[i].m_Iambient, 1.0f), sceneLights[i].m_Idiffuse)
			);
		}
		else
			break;
	}

	return result;
}