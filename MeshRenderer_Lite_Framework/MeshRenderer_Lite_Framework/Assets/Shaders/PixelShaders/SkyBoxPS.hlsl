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
	//From the SkyBoxVS.hlsl, the worldPos variable is used to store 
	//the model-space coordinates of the skybox
	return skyBoxTextures.Sample(textureSampler, pixel.worldPos.xyz);
}