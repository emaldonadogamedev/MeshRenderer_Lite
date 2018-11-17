#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

RasterizerState NoCull
{
	CullMode = None;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	DepthFunc = LESS_EQUAL;
};

float4 main(PixelInputType pixel) : SV_TARGET
{
	float3 normal = normalsRT.Sample(textureSamplerWrap, pixel.uv).xyz;
	
	float2 uv = float2(0.5f - (atan2(normal.z, normal.x) / TWO_PI), acos(normal.y) / PI );
	
	return iblMap2D.Sample(textureSamplerWrap, uv);
}