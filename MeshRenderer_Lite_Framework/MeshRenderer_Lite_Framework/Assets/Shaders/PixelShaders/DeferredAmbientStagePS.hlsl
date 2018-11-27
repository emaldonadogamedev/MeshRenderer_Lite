#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		float4 diff = diffuseRT.Sample(textureSamplerWrap, pixel.uv);
		
		//Alpha < 0.f means that the pixel belongs to the skybox, no light should be calculated here
		if (diff.w < 0.f)
			return float4(diff.xyz,1);

		float4 pos = positionRT.Sample(textureSamplerWrap, pixel.uv);

		float4 resultColors[G_DEBUG_COUNT];

		if (gIsUsingIBL)
		{
			float3 normal = normalsRT.Sample(textureSamplerWrap, pixel.uv).xyz;
			float2 uv = SphericalUVMapping(normal);
			float3 irrColor = irradianceMap2D.Sample(textureSamplerWrap, uv).xyz;

			///////////////////////////////////////////////////////////////////////////////////////////////////
			// Diffuse part of IBL
			//Tone down the HDR color to 0 to 1 format
			float4 diffuse = (diff / PI) * float4(irrColor, 1.0f);

			///////////////////////////////////////////////////////////////////////////////////////////////////
			// Specular part of IBL
			float3 specular = float3(0, 0, 0);
			uint numStructs; //number of elements in weights buffer
			uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
			sampleWeights.GetDimensions(numStructs, stride);

			float4 KSandNS = specularAndNsRT.Sample(textureSamplerWrap, pixel.uv);
			float ns = KSandNS.w;
			float3 viewVec = normalize(cameraPosition.xyz - pos.xyz);
			
			//Needed vectors for reflection
			float3 R = (2.0f * (dot(normal, viewVec)) * normal) - viewVec;
			float3 A = normalize( cross(float3(0,1.f, 0), R ));
			float3 B = normalize( cross(R, A));

			float2 distortedUVs;
			float3 L;
			float3 wk; // sample light direction
			float3 H; // to calculate half-vector
			float NdotWk; // to calculate wk dot N
			float WkDotH, WkDotHSquare; // to calculate wk dot H
			float G; // G term in BRDF
			float oneMinusWkDotH; //temp storage to avoid extra calculations
			float3 F; //BRDF F term
			float2 sampleUV; // temp storage to convert wk to UV coords
			float3 radianceSample; // temp storage to sample from the radiance texture
			
			//To Calculate D term of BRDF
			const float Dterm1 = (ns + 2.0f) / TWO_PI;
			float Dterm2;
			float D;

			//To help Calculate mip level
			uint iblMapW, iblMapH;
			iblMap2D.GetDimensions(iblMapW, iblMapH);
			const float wTimeHbySampleCount = float(iblMapW * iblMapH) / float(numStructs);

			[loop]
			for (uint i = 0; i < numStructs; ++i)
			{
				distortedUVs = float2(sampleWeights[i].x, acos(pow(sampleWeights[i].y, 1.f / (ns + 1.f))) / PI);
				L = UVtoNdirection(distortedUVs);				

				wk = normalize((L.x * A) + (L.y * B) + (L.z * R));
				H = normalize(wk + viewVec);
				NdotWk = max(dot(normal, wk), 0.f);
				
				WkDotH = max(dot(wk, H), 0.f);
				WkDotHSquare = WkDotH <= 0.0001f ? 0.0001f : WkDotH * WkDotH;
				G = (1.f / WkDotHSquare) / 4.f;
				oneMinusWkDotH = (1.f - WkDotH);
				F = KSandNS.xyz + (float3(1,1,1) - KSandNS.xyz) * 
					(oneMinusWkDotH * oneMinusWkDotH * oneMinusWkDotH * oneMinusWkDotH * oneMinusWkDotH);
				
				sampleUV = SphericalUVMapping(wk);

				Dterm2 = pow(max(dot(normal, H), 0.f), ns);
				D = Dterm1 * Dterm2;

				float mipLevel = 0.5f * log2(wTimeHbySampleCount) - 0.5f * log2(D / 4.f);

				//radianceSample = iblMap2D.Sample(textureSamplerWrap, sampleUV).xyz;
				radianceSample = iblMap2D.SampleLevel(textureSamplerWrap, sampleUV, mipLevel);
				specular += G * F * radianceSample * NdotWk;
			}
			
			//Divide by the amount of weights
			specular /= float(numStructs);

			float4 result = diffuse + float4(specular, 1);
			result = pow(result / (result + float4(1, 1, 1, 1)), toneMappingExtraExpControl / 2.2);

			// multiplying by the global ambient light for extra control to the final color
			resultColors[G_DEBUG_NONE] = saturate(float4(gGlobalAmbient, 1.f) * result);
		}
		else //If no IBL, use regular ambient calculation
		{
			resultColors[G_DEBUG_NONE] = float4(gGlobalAmbient, 1.f) * diff;
		}

		resultColors[G_DEBUG_POSITION] = pos;

		float d = pos.w;
		resultColors[G_DEBUG_DEPTH] = float4(d, d, d, 1.0f);

		resultColors[G_DEBUG_UV_COORDS] = float4(pixel.uv, 0, 1.0f);
		resultColors[G_DEBUG_NORMALS] = normalsRT.Sample(textureSamplerWrap, pixel.uv);
		resultColors[G_DEBUG_DIFFUSE] = diff;
		resultColors[G_DEBUG_SPECULAR] = specularAndNsRT.Sample(textureSamplerWrap, pixel.uv);

		return resultColors[gDebugInfoType];
}