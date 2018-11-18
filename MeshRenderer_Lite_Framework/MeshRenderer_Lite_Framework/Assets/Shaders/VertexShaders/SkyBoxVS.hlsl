#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex)
{
	PixelInputType result = (PixelInputType)0;

	float4 pos = float4(vertex.position * 1000.f, 1.0f);

	//This is going to be the texture coordinates used as the texture cube
	result.worldPos = pos;

	float4x4 newViewMat = viewMtx;

	newViewMat[0][3] = newViewMat[3][0] = 0.0f;
	newViewMat[1][3] = newViewMat[3][1] = 0.0f;
	newViewMat[2][3] = newViewMat[3][2] = 0.0f;
	newViewMat[3][3] = 1.0f;

	//result.position = float4(mul(pos.xyz, (float3x3)viewMtx), 1.0f);
	result.position = mul(pos, newViewMat);
	result.position = mul(result.position, projectionMtx);

	//Since it's a sky box, we need to make sure that the position 
	// in Z is at the far plane. So now, Z / W is guaranteed to be 1.0
	result.position = result.position.xyww;

	result.uv = vertex.uv;

	return result;
}