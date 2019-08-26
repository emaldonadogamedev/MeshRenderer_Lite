#include "../TextureShaderIncludes.hlsli"

Texture2D inputTexture2D: register(t0);
RWTexture2D<float4> outputTexture2D : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID )
{
	outputTexture2D[dispatchThreadId.xy] = inputTexture2D[dispatchThreadId.xy];
}