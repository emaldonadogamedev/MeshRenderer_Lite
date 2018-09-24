#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

GBufferOutput main(PixelInputType pixel)
{
	GBufferOutput output = (GBufferOutput)0;
	output.Position.xyz = pixel.worldPos.xyz;
	output.Position.w = pixel.position.z / pixel.position.w;

	output.Normal = abs(float4(pixel.normal, 0.f));

	//Get the diffuse from the 
	uint textWidth, textHeight;
	diffTexture.GetDimensions(textWidth, textHeight);
	output.Diffuse = meshMaterial.useDiffuseTexture != 0 && (textWidth > 0 && textHeight > 0) ?
			diffTexture.Sample(textureSamplerWrap, pixel.uv)
			: meshMaterial.diffuseKd;

	output.SpecularAndNS.xyz = meshMaterial.specularKs.xyz;
	output.SpecularAndNS.w = meshMaterial.specularPowerNs;

	return output;
}