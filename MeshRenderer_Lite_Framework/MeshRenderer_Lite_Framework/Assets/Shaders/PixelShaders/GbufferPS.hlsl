#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

GBufferOutput main(PixelInputType pixel)
{
	GBufferOutput output = (GBufferOutput)0;
	output.Position.xyz = pixel.worldPos.xyz;
	output.Position.w = pixel.position.w / 100.0f;

	//to measuere texture dimensions
	uint textWidth, textHeight;
	normalTexture.GetDimensions(textWidth, textHeight);

	if (meshMaterial.useNormalMap != 0 && (textWidth > 0 && textHeight > 0))
	{
		//Calculate the new normal if we're using normal mapping
		//Expand the range of the normal value from (0, +1) to (-1, +1).
		float4 bumpColor = normalTexture.Sample(textureSamplerWrap, pixel.uv);
		bumpColor = (bumpColor * 2.0f) - 1.0f;

		// Calculate the normal from the data in the bump map.
		float3 bumpNormal = (bumpColor.x * pixel.tangent) + (bumpColor.y * pixel.bitangent) + (bumpColor.z * pixel.normal);

		// Normalize the resulting bump normal.
		bumpNormal = normalize(bumpNormal);

		output.Normal = float4(bumpNormal, 0.f);
	}
	else
	{
		output.Normal = float4(pixel.normal, 0.f);
	}

	//Get the diffuse color
	diffTexture.GetDimensions(textWidth, textHeight);
	if (isSkydome)
	{
		float3 cubeUV = normalize(pixel.worldPos);
		float2 uv = SphericalUVMapping(cubeUV);

		float3 diff = diffTexture.Sample(textureSamplerWrap, uv).xyz;
		diff = pow(diff / (diff + float3(1.f, 1.f, 1.f)), toneMappingExtraExpControl / 2.2);

		output.Diffuse.xyz = diff;
		output.Diffuse.w = -1.f;
	}
	else 
	{
		if (meshMaterial.useDiffuseTexture != 0 && (textWidth > 0 && textHeight > 0))
		{
			output.Diffuse = diffTexture.Sample(textureSamplerWrap, pixel.uv) * meshMaterial.diffuseKd;
		}
		else
		{
			output.Diffuse = meshMaterial.diffuseKd;
		}
	}

	output.SpecularAndNS.xyz = meshMaterial.specularKs.xyz;
	output.SpecularAndNS.w = meshMaterial.specularPowerNs;

	return output;
}