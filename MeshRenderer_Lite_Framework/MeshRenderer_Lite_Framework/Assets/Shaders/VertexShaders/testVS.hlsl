cbuffer cbChangesEveryFrame : register(b0)
{
	matrix worldMtx;
};

cbuffer cbNeverChanges : register(b1)
{
	matrix viewMtx;
	matrix projectionMtx;
};


struct VertexWire
{
	float3 position     : POSITION;
	float4 color		: COLOR;
};

struct PixelWire
{
	float4 position     : SV_POSITION;
	float4 color		: COLOR;
};

PixelWire main(VertexWire vertex)
{
	PixelWire result = (PixelWire)0;

	result.position = mul(float4(vertex.position, 1.0f), worldMtx );
	result.position = mul(result.position, viewMtx );
	result.position = mul(result.position, projectionMtx );

	result.color = vertex.color;

	return result;
}