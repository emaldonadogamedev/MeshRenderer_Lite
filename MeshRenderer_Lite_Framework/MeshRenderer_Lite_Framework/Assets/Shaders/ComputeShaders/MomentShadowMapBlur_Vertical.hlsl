#include "../TextureShaderIncludes.hlsli"

struct WeightStruct
{
		float weight;
};
StructuredBuffer<WeightStruct> weights : register(u0);

Texture2D inputShadowMap: register(t0);
RWTexture2D<float4> outputShadowMap : register(t1);

[numthreads(1, 128, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
		uint numStructs; //number of elements in weights buffer
		uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
		weights.GetDimensions(numStructs, stride);

		uint outShadowMapWidth, outShadowMapHeight;
		outputShadowMap.GetDimensions(outShadowMapWidth, outShadowMapHeight);

		const uint2 pixelCoords = dispatchThreadId.xy;
		const float normalizedUV_x = float(pixelCoords.x) / float(outShadowMapWidth);

		uint halfSize = numStrucs / 2;
		float4 result = float4(0, 0, 0, 0);
		for (uint y = pixelCoords.y - halfSize, weightsIdx = 0; y <= (pixelCoords.y + halfSize); ++y, ++weightsIdx)
		{
				if (y >= 0 && y < outShadowMapHeight)
				{
						result += weights[weightsIdx] * inputShadowMap.Sample(textureSamplerWrap, float2(normalizedUV_x, float(y) / float(outShadowMapHeight)));
				}
		}

		outputShadowMap[pixelCoords] = saturate(result);
}