#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex)
{
	PixelInputType result = (PixelInputType)0;

	float4 pos = float4(vertex.position, 1.0f);

	if (isAnimated)
	{
		matrix boneTransform = boneMatrices[vertex.boneIDs.x] * vertex.boneWeights.x;
		boneTransform += boneMatrices[vertex.boneIDs.y] * vertex.boneWeights.y;
		boneTransform += boneMatrices[vertex.boneIDs.z] * vertex.boneWeights.z;
		boneTransform += boneMatrices[vertex.boneIDs.w] * vertex.boneWeights.w;
		pos = mul(pos, boneTransform);
	}

	result.worldPos = mul(pos, worldMtx);
	result.position = mul(result.worldPos, viewMtx);
	result.position = mul(result.position, projectionMtx);
	result.normal = normalize(mul(vertex.normal, (float3x3)worldMtx));
	result.tangent = normalize(mul(float4(vertex.tangent, 0.f), worldMtx));
	result.bitangent = normalize(mul(float4(vertex.bitangent, 0.f), worldMtx));
	result.uv = vertex.uv;
	result.color = vertex.color;
	//JUST AS A TEST, look closely at the moving vertices
	//pos = normalize(pos);
	//result.color = pos;

	return result;
}