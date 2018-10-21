#include "../TextureShaderIncludes.hlsli"

RWTexture2D<float4> inputShadowMap: register(u0);
RWTexture2D<float4> outputShadowMap : register(u1);
StructuredBuffer <float> weights : register(t1);


[numthreads(1, 128, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
		float testWeights[11] =
		{
			0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
		};

		uint numStructs; //number of elements in weights buffer
		uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
		weights.GetDimensions(numStructs, stride);

		uint outShadowMapWidth, outShadowMapHeight;
		outputShadowMap.GetDimensions(outShadowMapWidth, outShadowMapHeight);

		const uint2 pixelCoords = dispatchThreadId.xy;
		const float normalizedUV_x = float(pixelCoords.x) / float(outShadowMapWidth);

		const uint halfSize = 11 / 2;
		float4 result = float4(0, 0, 0, 0);
		for (uint y = pixelCoords.y - halfSize, weightsIdx = 0; y <= (pixelCoords.y + halfSize); ++y, ++weightsIdx)
		{
				if (y >= 0 && y < outShadowMapHeight)
				{
						result += testWeights[weightsIdx] * inputShadowMap[uint2(pixelCoords.x, y)];
				}
		}

		result.w = 1.0f;
		outputShadowMap[pixelCoords] = saturate(result);
}