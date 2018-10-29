#include "../TextureShaderIncludes.hlsli"

Texture2D inputShadowMap : register(t0);
RWTexture2D<float4> outputShadowMap : register(u0);
StructuredBuffer <float> weights : register(t1);

groupshared float4 sharedMemFloats[128 + 21];

[numthreads(128, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
		uint numStructs; //number of elements in weights buffer
		uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
		weights.GetDimensions(numStructs, stride);

		const int halfSize = int(numStructs) / 2;

		uint outShadowMapWidth, outShadowMapHeight;
		outputShadowMap.GetDimensions(outShadowMapWidth, outShadowMapHeight);

		const int2 pixelCoords = dispatchThreadId.xy;

		sharedMemFloats[pixelCoords.x] = inputShadowMap[pixelCoords + int2(-halfSize, 0)];
		if (pixelCoords.x < (halfSize * 2))
		{
			sharedMemFloats[pixelCoords.x + 128] = inputShadowMap[pixelCoords + int2(128 - halfSize, 0)];// read extra 2*w pixels
		}

		//wait for all threads to read
		//AllMemoryBarrier();
		AllMemoryBarrierWithGroupSync();
		//GroupMemoryBarrierWithGroupSync();

		float4 result = float4(0, 0, 0, 0);

		for (int i = -halfSize, w = 0; i <= halfSize; ++i, ++w)
		{
			result += weights[w] * sharedMemFloats[pixelCoords.x + i + halfSize];
		}

		outputShadowMap[pixelCoords] = result;
}