// Input from domain shader
struct DS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : UV;
	// TODO: change/add other stuff
};

float4 main(DS_OUTPUT pixelInput) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}