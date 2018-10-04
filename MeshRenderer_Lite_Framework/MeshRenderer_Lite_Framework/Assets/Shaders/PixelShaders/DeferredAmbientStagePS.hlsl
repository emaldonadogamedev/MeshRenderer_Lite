#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
	float4 result = float4(0,0,0,0);

		for (unsigned int i = 0; i < s_maxLights; ++i)
		{
				if (sceneLights[i].isTaken)
				{
						if (sceneLights[i].isActive)
						{
								result += sceneLights[i].m_Iambient * diffuseRT.Sample(textureSamplerWrap, pixel.uv);
						}
				}
				else
						break;
		}

	return result;
}