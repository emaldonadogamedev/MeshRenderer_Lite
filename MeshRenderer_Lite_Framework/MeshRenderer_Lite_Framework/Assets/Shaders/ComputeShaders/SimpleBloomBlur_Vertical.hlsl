#include "../TextureShaderIncludes.hlsli"

Texture2D inputBloomMap : register(t0);
RWTexture2D<float4> outputMap : register(u0);

[numthreads(1, 5, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };

	uint inMapWidth, inMapHeight;
	inputBloomMap.GetDimensions(inMapWidth, inMapHeight);

	float2 texOffset = float2(0.0f, 1.0f / float(inMapHeight) ); // gets size of single texel
	float2 uv = float2( float(dispatchThreadId.x) / float(inMapWidth),
		float(dispatchThreadId.y) / float(inMapHeight));

	float3 result = inputBloomMap[dispatchThreadId.xy].xyz * weights[0];

	[unroll]
	for (int i = 1; i < 5; ++i)
	{
		result += inputBloomMap[(uv + (texOffset * i)) * int2(inMapWidth, inMapHeight)].xyz * weights[i];
		result += inputBloomMap[(uv - (texOffset * i)) * int2(inMapWidth, inMapHeight)].xyz * weights[i];
	}

	outputMap[dispatchThreadId.xy] = float4(result, 1.0f);
}