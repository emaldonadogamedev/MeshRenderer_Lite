#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"



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

// Input from domain shader
struct DS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : UV;
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

    //return float4(pixelInput.vUV, 0, 0.5f);
    return saturate(s * testAirlightCoefficient * l * sceneLights[0].m_Idiffuse);
}