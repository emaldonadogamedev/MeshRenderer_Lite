#include "../TextureShaderIncludes.hlsli"

//Texture2D inputShadowMap : register(t0); //For some reason this crap doesn't work after running the horizontal blur shader
RWTexture2D<float4> outputShadowMap : register(u0);
StructuredBuffer <float> weights : register(t1);

groupshared float4 sharedMemFloats[128 + 21];

[numthreads(1, 128, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
		uint numStructs; //number of elements in weights buffer
		uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
		weights.GetDimensions(numStructs, stride);

		const int halfSize = int(numStructs) / 2;

		uint outShadowMapWidth, outShadowMapHeight;
		outputShadowMap.GetDimensions(outShadowMapWidth, outShadowMapHeight);

		const int2 pixelCoords = dispatchThreadId.xy;

		sharedMemFloats[pixelCoords.y] = inputShadowMap[pixelCoords + int2(0, -halfSize)];
		if (pixelCoords.y < (halfSize * 2))
		{
			sharedMemFloats[pixelCoords.y + 128] = inputShadowMap[pixelCoords + int2(0, 128 - halfSize)];// read extra 2*w pixels
		}

		//wait for all threads to read
		AllMemoryBarrierWithGroupSync();

		float4 result = float4(0, 0, 0, 0);

		{
		}

		outputShadowMap[pixelCoords] = result;
}