#include "../TextureShaderIncludes.hlsli"

RWTexture2D<float4> inputTexture2D: register(u0);
RWTexture2D<float4> outputTexture2D : register(u1);

[numthreads(32, 32, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID )
{
		outputTexture2D[dispatchThreadId.xy] = inputTexture2D[dispatchThreadId.xy];
}