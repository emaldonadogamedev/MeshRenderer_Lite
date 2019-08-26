#include "../TextureShaderIncludes.hlsli"

Texture2D inputTexture2D_0: register(t0);
Texture2D inputTexture2D_1: register(t1);
RWTexture2D<float4> outputTexture2D : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	outputTexture2D[dispatchThreadId.xy] = saturate(inputTexture2D_0[dispatchThreadId.xy] + inputTexture2D_1[dispatchThreadId.xy]);
}