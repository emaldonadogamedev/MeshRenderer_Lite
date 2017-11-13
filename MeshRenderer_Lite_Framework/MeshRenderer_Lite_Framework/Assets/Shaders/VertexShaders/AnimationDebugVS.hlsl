#include "../ShaderIncludes.hlsli"
#include "VertexShaderIncludes.hlsli"

struct BillboardVertex
{
	float4 position;
};

PixelInputType main(in uint vertexID:SV_VertexID)
{
	PixelInputType result = (PixelInputType)0;

	BillboardVertex vInforArr[4] = {
		{ -0.5f, -0.5f, 0, 1.0f },
		{ -0.5f,  0.5f, 0, 1.0f },
		{  0.5f,  0.5f, 0, 1.0f },
		{  0.5f, -0.5f, 0, 1.0f }
	};


	result.color = vertex.color;

	return result;
}