#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float4 main(PixelInputType pixel) : SV_TARGET
{
		float4 pos = positionRT.Sample(textureSamplerWrap, pixel.uv);
		float4 diff = diffuseRT.Sample(textureSamplerWrap, pixel.uv);
		float4 resultColors[G_DEBUG_COUNT];

		resultColors[G_DEBUG_NONE] = float4(gGlobalAmbient, 1.f) * diff;
		resultColors[G_DEBUG_POSITION] = pos;

		float d = pos.z / pos.w;
		resultColors[G_DEBUG_DEPTH] = float4(d, d, d, 1.0f);

		resultColors[G_DEBUG_UV_COORDS] = float4(pixel.uv, 0, 1.0f);
		resultColors[G_DEBUG_NORMALS] = normalsRT.Sample(textureSamplerWrap, pixel.uv);
		resultColors[G_DEBUG_DIFFUSE] = diff;
		resultColors[G_DEBUG_SPECULAR] = specularAndNsRT.Sample(textureSamplerWrap, pixel.uv);

		return resultColors[gDebugInfoType];
}