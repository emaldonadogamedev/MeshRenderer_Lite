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

float4 main(PixelWire pixel) : SV_TARGET
{
	return pixel.color;
}