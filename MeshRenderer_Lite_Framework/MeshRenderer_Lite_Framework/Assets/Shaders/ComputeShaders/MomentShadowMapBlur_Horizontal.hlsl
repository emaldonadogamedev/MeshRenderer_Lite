#include "../TextureShaderIncludes.hlsli"

Texture2D inputShadowMap : register(t0);
RWTexture2D<float4> outputShadowMap : register(u0);
StructuredBuffer <float> weights : register(t1);

groupshared float4 sharedMemFloats[128 + 51];

[numthreads(128, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID, uint3 groupThreadId : SV_GroupThreadID)
{
		uint numStructs; //number of elements in weights buffer
		uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
		weights.GetDimensions(numStructs, stride);

		const int halfSize = int(numStructs) / 2;

		uint outShadowMapWidth, outShadowMapHeight;
		outputShadowMap.GetDimensions(outShadowMapWidth, outShadowMapHeight);

		const int2 pixelCoords = dispatchThreadId.xy;

    sharedMemFloats[groupThreadId.x] = inputShadowMap[ int2(max(pixelCoords.x - halfSize, 0), pixelCoords.y) ];
		if (groupThreadId.x < (halfSize * 2))
		{
            // read extra 2*w pixels
            int2 coords = int2(min(pixelCoords.x + 128 - halfSize, outShadowMapWidth - 1), pixelCoords.y);

            sharedMemFloats[groupThreadId.x + 128] = inputShadowMap[coords];
		}

		//wait for all threads to read
		//AllMemoryBarrier();
		AllMemoryBarrierWithGroupSync();
		//DeviceMemoryBarrier();
		//GroupMemoryBarrierWithGroupSync();

		float4 result = float4(0, 0, 0, 0);

		for (int i = -halfSize, w = 0; i <= halfSize; ++i, ++w)
		{
			result += weights[w] * sharedMemFloats[groupThreadId.x + i + halfSize];
		}

		outputShadowMap[pixelCoords] = result;
}