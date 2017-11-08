#include "../ShaderIncludes.hlsli"

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
		//pos = mul(pos, Transpose(boneTransform));
		pos = mul(pos, boneTransform);

		//TODO: Remove this line since it's only for testing
		pos.w = 1.0f;
	}

	result.worldPos = mul(pos, worldMtx);
	result.position = mul(result.worldPos, viewMtx);
	result.position = mul(result.position, projectionMtx);

	result.color = color;

	return result;
}