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
	result.uv = vertex.uv;
	//JUST AS A TEST, look closely at the moving vertices
	//pos = normalize(pos);
	//result.color = pos;

	//TODO: GO BACK TO THIS!
	//result.color = vertex.color;

	return result;
}