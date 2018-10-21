#include "../TextureShaderIncludes.hlsli"

struct WeightStruct
{
		float weight;
};
StructuredBuffer<WeightStruct> weights : register(u0);

Texture2D inputShadowMap: register(t0);
RWTexture2D<float4> outputShadowMap : register(t1);

[numthreads(128, 1, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID)
{
		uint numStructs; //number of elements in weights buffer
		uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
		weights.GetDimensions(numStructs, stride);

		uint outShadowMapWidth, outShadowMapHeight;
		outputShadowMap.GetDimensions(outShadowMapWidth, outShadowMapHeight);

		const uint2 pixelCoords = dispatchThreadId.xy;
		const float normalizedUV_y = float(pixelCoords.y) / float(outShadowMapHeight);

		const uint halfSize = numStrucs / 2;
		float4 result = float4(0, 0, 0, 0);
		for (uint x = pixelCoords.x - halfSize, weightsIdx = 0; x <= (pixelCoords.x + halfSize); ++x, ++weightsIdx)
		{
				if (x >= 0 && x < outShadowMapWidth)
				{
						result += weights[weightsIdx] * inputShadowMap.Sample(textureSamplerWrap, float2(float(x) / float(outShadowMapWidth), normalizedUV_y));
				}
		}

		outputShadowMap[pixelCoords] = saturate(result);
}