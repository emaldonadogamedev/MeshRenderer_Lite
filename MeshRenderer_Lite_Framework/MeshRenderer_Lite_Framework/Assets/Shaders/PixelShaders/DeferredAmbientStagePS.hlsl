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
			//irrColor = pow(irrColor / (irrColor + float3(1.f, 1.f, 1.f)), toneMappingExtraExpControl / 2.2);
			float4 diffuse = (diff / PI) * float4(irrColor  * 10, 1.0f);

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
			float3 R = normalize( (2.0f * (dot(normal, viewVec)) * normal) - viewVec);
			float3 A = normalize( cross(float3(0,1.f, 0), R ));
			float3 B = normalize( cross(R, A));

			[unroll(40)]
			for (uint i = 0; i < numStructs; ++i)
			{
				float2 distortedUVs = float2(sampleWeights[i].x, acos(pow(abs(sampleWeights[i].y), 1.f / (ns + 1.f))) / PI);
				float3 L = UVtoNdirection(distortedUVs);
				float NdotL = max(dot(normal, L), 0.f);

				float3 wk = normalize( (L.x * A) + (L.y * B) + (L.z * R) );
				float3 H = normalize(L + viewVec);
				
				float LdotH = max(dot(L, H), 0.f);
				float G = (1.f /(LdotH * LdotH)) / 4.f;
				float oneMinusLdotH = (1.f - LdotH);
				float3 F = KSandNS.xyz + (float3(1,1,1) - KSandNS.xyz) * 
					(oneMinusLdotH * oneMinusLdotH * oneMinusLdotH * oneMinusLdotH *oneMinusLdotH);
				

				float2 sampleUV = SphericalUVMapping(wk);
				float3 radianceSample = iblMap2D.Sample(textureSamplerWrap, sampleUV).xyz;
				specular += G * F * radianceSample * NdotL;
			}
			
			//Divide by the amount of weights
			specular /= float(numStructs);

			resultColors[G_DEBUG_NONE] = saturate(diffuse + float4(specular, 1));

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