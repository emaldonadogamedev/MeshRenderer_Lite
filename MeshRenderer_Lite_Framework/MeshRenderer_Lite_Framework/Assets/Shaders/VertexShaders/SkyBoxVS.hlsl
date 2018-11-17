#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

PixelInputType main(VertexInputType vertex)
{
	PixelInputType result = (PixelInputType)0;

	float4 pos = float4(vertex.position * 100.0f, 1.0f);

	//This is going to be the texture coordinates used as the texture cube
	result.worldPos = pos;
	
	result.position = mul(pos, viewMtx);
	result.position = mul(result.position, projectionMtx);

	//Since it's a sky box, we need to make sure that the position 
	// in Z is at the far plane. So now, Z / W is guaranteed to be 1.0
	result.position = result.position.xyww;

	result.uv = vertex.uv;

	return result;
}