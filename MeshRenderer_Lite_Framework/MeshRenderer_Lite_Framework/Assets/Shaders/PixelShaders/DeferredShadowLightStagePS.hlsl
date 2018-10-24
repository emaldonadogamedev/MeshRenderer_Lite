#include "../ShaderIncludes.hlsli"
#include "../VertexShaders/VertexShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"
#include "LightingInclude.hlsli"

float det3(float3 a, float3 b, float3 c) // Determinant of a 3x3 passed in as three colum
{
		return a.x*(b.y*c.z - b.z*c.y) + a.y*(b.z*c.x - b.x*c.z) + a.z*(b.x*c.y - b.y*c.x);
}

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
														float lightDepthValue = (lightViewPosition.w);

														// Subtract the bias from the lightDepthValue.
														lightDepthValue -= bias;

														// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this 
														//pixel. If the light is in front of the object then light the pixel, if not then shadow this pixel since an object 
														//(occluder) is casting a shadow on it.
														if (lightDepthValue > depthValue)
														{
																lightIntensity = 0.f;
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

												float4 blurredDepthValue = shadowMaps[i][uint2(projectTexCoord.x * shadowMapW, projectTexCoord.y * shadowMapH)];
												float4 bPrime = (1.0f - 0.001f) * blurredDepthValue;
												bPrime += 0.001f * (float1x4(0.5f, 0.5f, 0.5f, 0.5f));

												matrix <float, 3, 3> lhsMatrix = 
												{ 
														1.0f, bPrime.x, bPrime.y,    // row 1
														bPrime.x, bPrime.y, bPrime.z, // row 2
														bPrime.y, bPrime.z, bPrime.w // row 3
												};

												const float3 A = float3(lhsMatrix[0][0], lhsMatrix[1][0], lhsMatrix[2][0]);
												const float3 B = float3(lhsMatrix[0][1], lhsMatrix[1][1], lhsMatrix[2][1]);
												const float3 C = float3(lhsMatrix[0][2], lhsMatrix[1][2], lhsMatrix[2][2]);

												// Calculate the depth of the light.
												float lightDepthValue = lightViewPosition.w;

												// Subtract the bias from the lightDepthValue.
												lightDepthValue -= bias;

												const float3 Z = float3(1.0f, lightDepthValue, lightDepthValue * lightDepthValue);

												const float d = det3(A, B, C);
												const float c1 = det3(Z, B, C) / d;
												const float c2 = det3(A, Z, C) / d;
												const float c3 = det3(A, B, Z) / d;
												
												//Solve using quadratic formula
												float z2, z3;
												const float denominator = 2.0f * c3;
												const float bSquared = c2 * c2;
												const float fourAC = 4.0f * c3 * c1;
												const float sqrRootResult = sqrt(bSquared - fourAC);

												z2 = ((-c2) - sqrRootResult) / denominator;
												z3 = ((-c2) + sqrRootResult) / denominator;

												z2 = min(z2, z3);
												z3 = max(z2, z3);

												if (lightDepthValue <= z2)
												{
														lightIntensity = 0.f;
												}
												else if (lightDepthValue <= z3)
												{
														const float num = (lightDepthValue*z3) - (bPrime.x * (lightDepthValue + z3)) + bPrime.y;
														const float den = (z3 - z2) * (lightDepthValue - z2);
														lightIntensity = num / den;
												}

												else
												{
														const float num = (z2*z3) - (bPrime.x * (z2 + z3)) + bPrime.y;
														const float den = (lightDepthValue - z2) * (lightDepthValue - z3);
														lightIntensity = 1.0f - (num / den);
												}

										}
								}
						}

						result += saturate(lightIntensity * CaculateBRDFLighting(position, normal, kd, float4(ksAndNs.xyz, 1.0f), ksAndNs.w, cameraPosition.xyz,
								sceneLights[i].m_position, float4(sceneLights[i].m_Iambient, 1.0f), sceneLights[i].m_Idiffuse, sceneLights[i].m_ConstantAttenuation,
								sceneLights[i].m_LinearAttenuation, sceneLights[i].m_QuadraticAttenuation));
				}
				else
						break;
		}

	return result;
}