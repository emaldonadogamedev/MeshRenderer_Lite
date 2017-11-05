#include "../ShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex)
{
	PixelInputType result = (PixelInputType)0;

	float4 pos = float4(vertex.position, 1.0f);

	if (vertex.boneWeights.x > 0.0f)
	{
		matrix boneTransform = boneMatrices[vertex.boneIDs.x] * vertex.boneWeights.x;
		boneTransform += boneMatrices[vertex.boneIDs.y] * vertex.boneWeights.y;
		boneTransform += boneMatrices[vertex.boneIDs.z] * vertex.boneWeights.z;
		boneTransform += boneMatrices[vertex.boneIDs.w] * vertex.boneWeights.w;
		mul(pos, boneTransform);
	}

	result.worldPos = mul(pos, worldMtx);
	result.position = mul(result.worldPos, viewMtx);
	result.position = mul(result.position, projectionMtx);

	result.color = color;

	return result;
}