#include "../TextureShaderIncludes.hlsli"

//Texture2D inputShadowMap : register(t0); //For some reason this crap doesn't work after running the horizontal blur shader
RWTexture2D<float4> inputShadowMap : register(u1);
RWTexture2D<float4> outputShadowMap : register(u0);
StructuredBuffer <float> weights : register(t1);


[numthreads(1, 128, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
		uint numStructs; //number of elements in weights buffer
		uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
		weights.GetDimensions(numStructs, stride);

		uint outShadowMapWidth, outShadowMapHeight;
		outputShadowMap.GetDimensions(outShadowMapWidth, outShadowMapHeight);

		const uint2 pixelCoords = dispatchThreadId.xy;

		const uint halfSize = numStructs / 2;
		float4 result = float4(0, 0, 0, 0);
		for (uint y = pixelCoords.y - halfSize, weightsIdx = 0; y <= (pixelCoords.y + halfSize); ++y, ++weightsIdx)
		{
				if (y >= 0 && y < outShadowMapHeight)
				{
						result += weights[weightsIdx] * inputShadowMap[uint2(pixelCoords.x, y)];
				}
		}

		outputShadowMap[pixelCoords] = result;
}