float CalculateAttenuation(in int lightType, in float d, in float C, in float L, in float Q) 
{
	return lightType == LT_DIRECTIONAL ? 1.0f :
		min(
				1.0f / (C + (L * d) + (Q * d * d)),
				1.0f
		);
}

float CalculateSpotlightEffect(int lightType, in float3 lightVec, in float3 lightSpotDir, in float outerAngle,
		in float innerAngle, in float ns)
{
		if (lightType != LT_SPOT || ns <= 0.0f)
		{
				return 1.0f;
		}
		const float3 D = normalize(-lightSpotDir);
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

float4 Fterm(in float4 Ks, in float3 L, in float3 H) 
{
		//(1?L?H)^5
		float temp = 1.0f - max(dot(L, H), 0.f);
		temp = temp * temp * temp * temp * temp;

		return Ks + ((float4(1, 1, 1, 1) - Ks) * temp);
}

float Dterm(in float3 N, in float3 H, in float roughness) 
{
		float term1 = (roughness + 2.0f) / TWO_PI;
		float term2 = pow(max(dot(N, H), 0.f), roughness);

		return term1 * term2;
}

float4 CaculateBRDFLighting(float3 vertexNormal, float4 Kd, float4 Ks, float ns, float3 viewVec, float3 lightVec, float4 Ia, float4 Id, bool isDeferred = true)
{
		const float vDotN = max(dot(viewVec, vertexNormal), 0);

		float4 diffuse, BRDF;

		//Ambient term
		float4 finalColor = isDeferred ? float4(0, 0, 0, 1.0f) : Ia * Kd;

		float LdotN = max(dot(lightVec, vertexNormal), 0.f);

		float3 H = normalize(lightVec + viewVec);
		float LdotH = max(dot(lightVec, H), 0.f);

		diffuse = Kd / PI;

		float4 F = Fterm(Ks, lightVec, H);
		float D = Dterm(vertexNormal, H, ns);

		float Gdenominator = LdotH * LdotH;
		float G = abs(Gdenominator) < 0.0001f ? 0.f : (1.0f / Gdenominator);

		float denominator = (4.0f * LdotN * vDotN);
		BRDF = denominator < 0.0001f ? float4(0,0,0,1.0f) : diffuse + (F * G * D) / denominator;

		finalColor += (
				((Id * max(dot(vertexNormal, lightVec), 0.f) * BRDF))
				);

		return finalColor;
}