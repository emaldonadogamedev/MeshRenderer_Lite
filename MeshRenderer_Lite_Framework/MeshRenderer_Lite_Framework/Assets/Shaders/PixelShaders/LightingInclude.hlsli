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

float4 CaculateBRDFLighting(float3 vertexPos, float3 vertexNormal,float3 cameraPos, float4 Kd, float4 Ks, Light light)
{
		const float3 viewVec = normalize(cameraPos - vertexPos);
		const float vDotN = max(dot(viewVec, vertexNormal), 0);
		float3 lightVec;

		float4 diffuse, BRDF;

		//Ambient term
		float4 finalColor = light.m_Iambient * Kd;

		lightVec = (light.m_position - vertexPos);
		float lightVecLength = length(lightVec);

		lightVec /= lightVecLength;

		float LdotN = max(dot(lightVec, vertexNormal), 0.f);

		float3 H = normalize(lightVec + viewVec);
		float LdotH = max(dot(lightVec, H), 0.f);

		diffuse = Kd / PI;

		float4 F = Fterm(Ks, lightVec, H);
		float D = Dterm(vertexNormal, H, light.roughness);
		float G = 1.0f / (LdotH * LdotH);

		BRDF = diffuse + (F * G * D) / (4.0f * LdotN * vDotN);

		float attenuation = CalculateAttenuation(light.m_lightType, lightVecLength, light.m_ConstantAttenuation,
				light.m_LinearAttenuation, light.m_QuadraticAttenuation);

		finalColor += (
				attenuation *
				((light.m_Idiffuse * max(dot(vertexNormal, lightVec), 0.f) * BRDF))
				);

		return finalColor;
}