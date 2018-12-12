#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

/*auxiliary functions*/
float A0(float I0, float beta, float Dsv, float gamma)
{
    return (beta * I0 * exp(-beta * Dsv * cos(gamma))) / (2 * PI * Dsv * sin(gamma));
}
float A1(float beta, float Dsv, float gamma)
{
    return beta * Dsv * sin(gamma);
}

/*functions to compute each radiance component seperately*/
float AirLight(float lightIntensity,
		float beta,
		float Dsv,
		float Dvp,
		float gammasv,
		float f_XOffset,
		float f_XScale,
		float f_YOffset,
		float f_YScale)
{
    const float PI = 3.1415926535;
    float u = A1(beta, Dsv, gammasv);
    float v1 = 0.25 * PI + 0.5 * atan((Dvp - Dsv * cos(gammasv)) / (Dsv * sin(gammasv)));
    float v2 = 0.5 * gammasv;
			
    float4 f_1 = volumeLight_F_512.Sample(textureSamplerWrap, float2((v1 - f_XOffset) * f_XScale, (u - f_YOffset) * f_YScale));
    float4 f_2 = volumeLight_F_512.Sample(textureSamplerWrap, float2((v2 - f_XOffset) * f_XScale, (u - f_YOffset) * f_YScale));
    return A0(lightIntensity, beta, Dsv, gammasv) * (f_1.x - f_2.x);
}

float Diffuse(float lightIntensity,
		float beta,
		float Kd,
		float Dsp,
		float Dvp,
		float thetas,
		float g_0XOffset,
		float g_0XScale,
		float g_0YOffset,
		float g_0YScale)
{
    const float PI = 3.1415926535;
	
    float t1 = exp(-beta * Dsp) * max(cos(thetas), 0) / Dsp;
    float4 t2 = beta * volumeLight_G_0.Sample(textureSamplerWrap, float2((beta * Dsp - g_0XOffset) * g_0XScale, (thetas - g_0YOffset) * g_0YScale)) / (2 * PI);
    return (t1 + t2.x) * exp(-beta * Dvp) * Kd * lightIntensity / Dsp;
}

float Specular(float lightIntensity,
		float beta,
		float Ks,
		float Dsp,
		float Dvp,
		float thetas_,
		float g_20XOffset,
		float g_20XScale,
		float g_20YOffset,
		float g_20YScale)
{
    const float PI = 3.1415926535;

    float t1 = exp(-beta * Dsp) * pow(max(cos(thetas_), 0), 20) / Dsp;
    float4 t2 = beta * volumeLight_G_20.Sample(textureSamplerWrap, float2((beta * Dsp - g_20XOffset) * g_20XScale, (thetas_ - g_20YOffset) * g_20YScale)) / (2 * PI);
    return (t1 + t2.x) * exp(-beta * Dvp) * Ks * lightIntensity / Dsp;
}

cbuffer LightVolumeProperties : register(b10)
{
    float3 volumeLightPos;
    float volumeLightfarPlane;

    float3 volumeLightUpVector;
    float volumeLightFOV;

    float3 volumeLightForwardVector;
    float volumeLightShadowMapWidth;

    float3 volumeLightRightVector;
    float volumeLightShadowMapHeight;

    float4 testAirlightCoefficient;
};

struct DS_OUTPUT
{
    float3 vWorldPos : WORLDPOS;
    float4 vPosition : SV_POSITION;
    float2 vUV : UV;
    float4 adjacentPoint : ADJ_POINT;
	// TODO: change/add other stuff
};

float4 main(DS_OUTPUT pixelInput, bool isFrontFace : SV_IsFrontFace) : SV_TARGET
{
    const float s = isFrontFace ? 1.f : -1.f;

    //move the camera position to proj space, same as the light volume position
    float4 camPos = mul(cameraPosition, viewMtx);
    camPos = mul(camPos, projectionMtx);

    //return a VEEEEEEEEEEEEERY GROSSLY Approximated air contribution
    float l = length(pixelInput.vPosition - camPos);

    return float4(pixelInput.vUV, 0, 0.5f);
    return saturate(s * testAirlightCoefficient * l * sceneLights[0].m_Idiffuse);

    ///////////////////////////////////////////////////////////////////////

    
    float4 OUT;
	
	//preparing parameters
    float3 N = normalize(isFrontFace ? );
    //float3 N = normalize(normal);
    float3 V = normalize(pixelInput.vWorldPos - cameraPosition.xyz);
    float3 S = normalize(sceneLights[0].m_position.xyz - cameraPosition.xyz);
    float3 L = normalize(sceneLights[0].m_position.xyz - pixelInput.vWorldPos);
    float3 RV = reflect(V, N);

    float Dvp = length(cameraPosition.xyz - pixelInput.vWorldPos);
    float Dsv = length(sceneLights[0].m_position.xyz - cameraPosition.xyz);
    float Dsp = length(sceneLights[0].m_position.xyz - pixelInput.vWorldPos);
    float gamma = acos(dot(S, V));
    float thetas = acos(dot(N, L));
    float thetas_ = acos(dot(L, RV));

	//compute airlight, diffuse and specular color seperately using our model in the paper
    //float airlight = AirLight(sceneLights[0].m_Idiffuse.x, beta, Dsv, Dvp, gamma, f_XOffset, f_XScale, f_YOffset, f_YScale);
    //float diffuse = Diffuse(sceneLights[0].m_Idiffuse.x, beta, Kd, Dsp, Dvp, thetas, g_0XOffset, g_0XScale, g_0YOffset, g_0YScale);
    //float specular = Specular(sceneLights[0].m_Idiffuse.x, beta, Ks, Dsp, Dvp, thetas_, g_20XOffset, g_20XScale, g_20YOffset, g_20YScale);

    //compute airlight, diffuse and specular color seperately using our model in the paper
    float airlight = AirLight(sceneLights[0].m_Idiffuse.x, 1.0f, Dsv, Dvp, gamma, 0.f , 1, 0, 1);
    float diffuse = Diffuse(sceneLights[0].m_Idiffuse.x, 1.0f, 1, Dsp, Dvp, thetas, 0, 1, 0, 1);
    float specular = Specular(sceneLights[0].m_Idiffuse.x, 1.0f, 1, Dsp, Dvp, thetas_, 0, 1, 0, 1);
	
	//diffuse color-red, airlight color-white, specular color-white
    OUT = float4(diffuse + airlight + specular, specular + airlight, specular + airlight, 0);
    return OUT;

}