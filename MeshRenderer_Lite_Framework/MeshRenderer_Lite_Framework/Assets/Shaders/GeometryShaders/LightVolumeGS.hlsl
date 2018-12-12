struct DS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : UV;
    float4 adjacentPoint : ADJ_POINT;
	// TODO: change/add other stuff
};

struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(3)]
void main(
	triangle DS_OUTPUT input[3],
	inout TriangleStream< GSOutput > output
)
{
    for (uint i = 0; i < 3; i++)
    {
        GSOutput element;
        element.pos = input[i].vPosition;
        output.Append(element);
    }
}