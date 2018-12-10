#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex, uint vertexID : SV_VERTEXID)
{
    PixelInputType result = (PixelInputType) 0;

    const float heightFar = 2.0f * tan(volumeLightFOV / 2.0f) * volumeLightfarPlane;
    const float widthFar = heightFar; // because aspect ratio is 1

    float3 deltaFloat3_X = volumeLightRightVector * widthFar / 2.0f;
    float3 deltaFloat3_Y = volumeLightUpVector * heightFar / 2.0f;
    float3 farPlanePos = volumeLightPos + (volumeLightForwardVector * volumeLightfarPlane);

    float3 worldSpacePos[4];
    worldSpacePos[0] = farPlanePos - deltaFloat3_Y - deltaFloat3_X;
    worldSpacePos[1] = farPlanePos + deltaFloat3_Y - deltaFloat3_X;
    worldSpacePos[2] = farPlanePos + deltaFloat3_Y + deltaFloat3_X;
    worldSpacePos[3] = farPlanePos - deltaFloat3_Y + deltaFloat3_X;

    result.worldPos = float4(worldSpacePos[vertexID], 1.0f);
    result.position = mul(result.worldPos, viewMtx);
    result.position = mul(result.position, projectionMtx);

    result.color = float4(1, 0, 0, 1);

    return result;
}