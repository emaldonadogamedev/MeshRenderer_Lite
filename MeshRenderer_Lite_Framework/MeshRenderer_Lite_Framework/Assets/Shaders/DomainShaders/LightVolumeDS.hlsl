#include "../ShaderIncludes.hlsli"
#include "../TextureShaderIncludes.hlsli"

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
};

struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
    float2 vUV : UV;
    float4 adjacentPoint : ADJ_POINT;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
    float3 vPosition : WORLDPOS;
    float2 uv : UV;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[4] : SV_TessFactor; // e.g. would be [4] for a quad domain
    float InsideTessFactor[2] : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 4

//Domain shader entry point
[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	//Output.vPosition = float4(
	//	patch[0].vPosition*domain.x+patch[1].vPosition*domain.y+patch[2].vPosition*domain.z,1);
    float3 v1 = lerp(patch[1].vPosition, patch[2].vPosition, domain.x);
    float3 v2 = lerp(patch[0].vPosition, patch[3].vPosition, domain.x);

    float4 pos = float4(lerp(v1, v2, domain.y), 1.0f);

    //Use the Shadow map to raise the point
    float shadowMapFactor = shadowMaps[0][int2(domain.x * volumeLightShadowMapWidth, domain.y * volumeLightShadowMapHeight)].w;
    pos = float4(pos.xyz + (volumeLightForwardVector * volumeLightfarPlane * shadowMapFactor), 1.0f);

    pos = mul(pos, viewMtx);
    pos = mul(pos, projectionMtx);

    Output.vPosition = pos;
    Output.vUV = domain;

    //TODO: finish this crap
    //naive way of telling the geom. shader that this is a corner point :(
    //if (domain.x == 0.f && domain.y == 0.f)
    //{
    //    Output.adjacentPoint.xyz = Output.vPosition.xyz;
    //}

    return Output;
}
