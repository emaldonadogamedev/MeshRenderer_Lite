Texture2D inputShadowMap: register(t0);
StructuredBuffer<float> weights : register(u0);

RWTexture2D<float4> outputShadowMap : register(t1);

[numthreads(128, 1, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID)
{
		uint2 pixelId = dispatchThreadId.xy;
}