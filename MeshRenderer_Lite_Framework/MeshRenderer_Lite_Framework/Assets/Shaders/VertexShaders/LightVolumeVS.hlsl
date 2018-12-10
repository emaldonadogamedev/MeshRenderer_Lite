#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    float4 worldPos : WORLDPOS;
    float2 uv : UV;
};

VS_CONTROL_POINT_OUTPUT main(VertexInputType vertex, uint vertexID : SV_VERTEXID)
{
    VS_CONTROL_POINT_OUTPUT result = (VS_CONTROL_POINT_OUTPUT) 0;

    const float heightFar = 2.0f * tan(volumeLightFOV / 2.0f) * volumeLightfarPlane;
    const float widthFar = heightFar; // because aspect ratio is 1

    float3 deltaFloat3_X = volumeLightRightVector * widthFar / 2.0f;
    float3 deltaFloat3_Y = volumeLightUpVector * heightFar / 2.0f;
    float3 farPlanePos = volumeLightPos + (volumeLightForwardVector * volumeLightfarPlane);

    VS_CONTROL_POINT_OUTPUT worldSpacePos[4];
    worldSpacePos[0].worldPos = float4(farPlanePos - deltaFloat3_Y - deltaFloat3_X, 1.0f);
    worldSpacePos[0].uv = float2(0.0f, 1.0f);

    worldSpacePos[1].worldPos = float4(farPlanePos + deltaFloat3_Y - deltaFloat3_X, 1.0f);
    worldSpacePos[1].uv = float2(0.0f, 0.0f);

    worldSpacePos[2].worldPos = float4(farPlanePos + deltaFloat3_Y + deltaFloat3_X, 1.0f);
    worldSpacePos[2].uv = float2(1.0f, 0.0f);

    worldSpacePos[3].worldPos = float4(farPlanePos - deltaFloat3_Y + deltaFloat3_X, 1.0f);
    worldSpacePos[3].uv = float2(1.0f, 1.0f);

    result = worldSpacePos[vertexID];

    return result;
}