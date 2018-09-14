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
		result.position = mul(result.worldPos, lightViewMtx);
		result.position = mul(result.position, lightProjectionMtx);

		return result;
}